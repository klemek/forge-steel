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

void state_apply_event(SharedContext *context, StateConfig state_config,
                       MidiDevice midi, unsigned char code, float value) {
  unsigned int index, part;
  bool found;

  found = false;

  if (value > 0) {
    // PAGE CHANGE
    index = arr_uint_index_of(state_config.select_page_codes, code);
    if (index != ARRAY_NOT_FOUND) {
      context->page = index;
      found = true;
    }

    // TARGET CHANGE
    index = arr_uint_index_of(state_config.select_frag_codes, code);
    if (index != ARRAY_NOT_FOUND) {
      context->selected = index + 1;
      found = true;
    }

    // ITEM CHANGE
    index = arr_uint_index_of(state_config.select_item_codes, code);
    if (index != ARRAY_NOT_FOUND) {
      context->state[context->selected - 1] =
          context->page * state_config.select_item_codes.length + index;
      found = true;
    }

    // ACTIVE CHANGE
    index = arr_uint_index_of(state_config.src_active_codes, code);
    if (index != ARRAY_NOT_FOUND) {
      part = arr_uint_remap_index(state_config.src_active_offsets, &index);
      context->active[part] = index;
      found = true;
    }

    if (!found) {
      log_trace("unknown midi: %d %.2f", code, value);
    }
  }

  midi_write(midi, code, value);
  // TODO
}

bool state_background_midi_write(
    SharedContext *context, __attribute__((unused)) StateConfig state_config,
    __attribute__((unused)) MidiDevice midi) {
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