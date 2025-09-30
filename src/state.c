#include <log.h>

#include "arr.h"
#include "config.h"
#include "config_file.h"
#include "midi.h"
#include "rand.h"
#include "state.h"
#include "types.h"

StateConfig state_parse_config(ConfigFile config) {
  unsigned int i, j, offset, total;
  StateConfig state_config;
  UintArray tmp_counts;
  char name[256];

  state_config.select_page_codes.length =
      config_file_get_int(config, "SELECT_PAGE_COUNT", 0);

  for (i = 0; i < state_config.select_page_codes.length; i++) {
    sprintf(name, "SELECT_PAGE_%d", i + 1);
    state_config.select_page_codes.values[i] =
        config_file_get_int(config, name, UNSET_MIDI_CODE);
  }

  state_config.select_item_codes.length =
      config_file_get_int(config, "SELECT_ITEM_COUNT", 0);

  for (i = 0; i < state_config.select_item_codes.length; i++) {
    sprintf(name, "SELECT_ITEM_%d", i + 1);
    state_config.select_item_codes.values[i] =
        config_file_get_int(config, name, UNSET_MIDI_CODE);
  }

  state_config.state_max = state_config.select_page_codes.length *
                           state_config.select_item_codes.length;

  state_config.select_frag_codes.length =
      config_file_get_int(config, "FRAG_COUNT", 1);

  for (i = 0; i < state_config.select_frag_codes.length; i++) {
    sprintf(name, "SELECT_FRAG_%d", i + 1);
    state_config.select_frag_codes.values[i] =
        config_file_get_int(config, name, UNSET_MIDI_CODE);
  }

  state_config.src_count = tmp_counts.length =
      state_config.src_active_offsets.length = state_config.src_offsets.length =
          config_file_get_int(config, "SRC_COUNT", 0);

  total = 0;
  for (i = 0; i < tmp_counts.length; i++) {
    sprintf(name, "SRC_%d_ACTIVE_COUNT", i + 1);
    tmp_counts.values[i] = config_file_get_int(config, name, 0);
    state_config.src_active_offsets.values[i] = total;
    total += tmp_counts.values[i];
  }

  state_config.src_active_codes.length = total;

  for (i = 0; i < tmp_counts.length; i++) {
    for (j = 0; j < tmp_counts.values[i]; j++) {
      sprintf(name, "SRC_%d_ACTIVE_%d", i + 1, j + 1);
      state_config.src_active_codes
          .values[state_config.src_active_offsets.values[i] + j] =
          config_file_get_int(config, name, UNSET_MIDI_CODE);
    }
  }

  total = 0;
  for (i = 0; i < tmp_counts.length; i++) {
    sprintf(name, "SRC_%d_COUNT", i + 1);
    tmp_counts.values[i] = config_file_get_int(config, name, 0);
    state_config.src_offsets.values[i] = total;
    total += tmp_counts.values[i];
  }

  state_config.src_codes.length = total * 3;

  for (i = 0; i < tmp_counts.length; i++) {
    offset = state_config.src_offsets.values[i];
    for (j = 0; j < tmp_counts.values[i]; j++) {
      sprintf(name, "SRC_%d_%d_X", i + 1, j + 1);
      state_config.src_codes.values[(offset + j) * 3] =
          config_file_get_int(config, name, UNSET_MIDI_CODE);

      sprintf(name, "SRC_%d_%d_Y", i + 1, j + 1);
      state_config.src_codes.values[(offset + j) * 3 + 1] =
          config_file_get_int(config, name, UNSET_MIDI_CODE);

      sprintf(name, "SRC_%d_%d_Z", i + 1, j + 1);
      state_config.src_codes.values[(offset + j) * 3 + 2] =
          config_file_get_int(config, name, UNSET_MIDI_CODE);
    }
  }

  state_config.fader_codes.length =
      config_file_get_int(config, "FADER_COUNT", 0);

  for (i = 0; i < state_config.fader_codes.length; i++) {
    sprintf(name, "FADER_%d", i + 1);
    state_config.fader_codes.values[i] =
        config_file_get_int(config, name, UNSET_MIDI_CODE);
  }

  state_config.tap_tempo_code =
      config_file_get_int(config, "TAP_TEMPO", UNSET_MIDI_CODE);

  return state_config;
}

static void safe_midi_write(MidiDevice midi, unsigned int code,
                            unsigned char value) {
  if (code != UNSET_MIDI_CODE) {
    midi_write(midi, code, value);
  }
}

static void update_page(SharedContext *context, StateConfig state_config,
                        MidiDevice midi) {
  unsigned int i, page_item_min, page_item_max;
  // SHOW PAGE
  for (i = 0; i < state_config.select_page_codes.length; i++) {
    safe_midi_write(midi, state_config.select_page_codes.values[i],
                    i == context->page ? MIDI_MAX : 0);
  }

  // SHOW PAGE ITEM
  page_item_min = state_config.select_item_codes.length * context->page;
  page_item_max = page_item_min + state_config.select_item_codes.length;

  if (context->state[context->selected] >= page_item_min &&
      context->state[context->selected] < page_item_max) {
    for (i = 0; i < state_config.select_item_codes.length; i++) {
      safe_midi_write(midi, state_config.select_item_codes.values[i],
                      i == context->state[context->selected] - page_item_min
                          ? MIDI_MAX
                          : 0);
    }
  } else {
    for (i = 0; i < state_config.select_item_codes.length; i++) {
      safe_midi_write(midi, state_config.select_item_codes.values[i], 0);
    }
  }
}

static void update_selected(SharedContext *context, StateConfig state_config,
                            MidiDevice midi) {
  unsigned int i;

  for (i = 0; i < state_config.select_frag_codes.length; i++) {
    safe_midi_write(midi, state_config.select_frag_codes.values[i],
                    i == context->selected ? MIDI_MAX : 0);
  }
}

void state_apply_event(SharedContext *context, StateConfig state_config,
                       MidiDevice midi, unsigned char code,
                       unsigned char value) {
  unsigned int index, part;
  bool found;

  found = false;

  // PAGE CHANGE
  index = arr_uint_index_of(state_config.select_page_codes, code);
  if (index != ARRAY_NOT_FOUND) {
    found = true;
    if (value > 0) {
      context->page = index;
      update_page(context, state_config, midi);
    }
  }

  // TARGET CHANGE
  index = arr_uint_index_of(state_config.select_frag_codes, code);
  if (index != ARRAY_NOT_FOUND) {
    found = true;
    if (value > 0) {
      context->selected = index;
      update_page(context, state_config, midi);
      update_selected(context, state_config, midi);
    }
  }

  // ITEM CHANGE
  index = arr_uint_index_of(state_config.select_item_codes, code);
  if (index != ARRAY_NOT_FOUND) {
    found = true;
    if (value > 0) {
      context->state[context->selected] =
          context->page * state_config.select_item_codes.length + index;
      update_page(context, state_config, midi);
    }
  }

  // ACTIVE CHANGE
  index = arr_uint_index_of(state_config.src_active_codes, code);
  if (index != ARRAY_NOT_FOUND) {
    found = true;
    if (value > 0) {
      part = arr_uint_remap_index(state_config.src_active_offsets, &index);
      context->active[part] = index;
      // TODO things
    }
  }

  // TODO values
  if (!found) {
    log_trace("unknown midi: %d %d", code, value);
    midi_write(midi, code, value);
  } else {
    log_trace("midi: %d %d", code, value);
  }
}

bool state_background_midi_write(SharedContext *context,
                                 StateConfig state_config, MidiDevice midi) {
  pid_t pid;

  pid = fork();
  if (pid < 0) {
    log_error("Could not create subprocess");
    return false;
  }
  if (pid == 0) {
    return true;
  }
  log_info("(state) background writing started (pid: %d)", pid);

  update_page(context, state_config, midi);
  update_selected(context, state_config, midi);

  while (!context->stop) {
    // TODO tap tempo and more
  }

  log_info("(state) background writing stopped by main thread (pid: %d)", pid);
  return false;
}

void state_randomize(SharedContext *context, StateConfig state_config) {
  unsigned int i;

  for (i = 0; i < state_config.select_frag_codes.length; i++) {
    context->state[i] = rand_uint(state_config.state_max);
  }
}