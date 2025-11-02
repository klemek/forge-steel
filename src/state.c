#include <log.h>
#include <stdio.h>

#include "arr.h"
#include "config.h"
#include "config_file.h"
#include "file.h"
#include "midi.h"
#include "rand.h"
#include "state.h"
#include "tempo.h"
#include "types.h"

StateConfig state_parse_config(ConfigFile config) {
  unsigned int i, j, offset, count;
  StateConfig state_config;
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

  state_config.midi_active_counts.length = state_config.midi_counts.length =
      state_config.midi_active_offsets.length =
          state_config.midi_offsets.length =
              state_config.values_offsets.length =
                  config_file_get_int(config, "MIDI_COUNT", 0);

  count = 0;
  for (i = 0; i < state_config.midi_active_counts.length; i++) {
    sprintf(name, "MIDI_%d_ACTIVE_COUNT", i + 1);
    state_config.midi_active_counts.values[i] =
        config_file_get_int(config, name, 1);
    state_config.midi_active_offsets.values[i] = count;
    count += state_config.midi_active_counts.values[i];
  }

  state_config.midi_active_codes.length = count;

  for (i = 0; i < state_config.midi_active_counts.length; i++) {
    for (j = 0; j < state_config.midi_active_counts.values[i]; j++) {
      sprintf(name, "MIDI_%d_ACTIVE_%d", i + 1, j + 1);
      state_config.midi_active_codes
          .values[state_config.midi_active_offsets.values[i] + j] =
          config_file_get_int(config, name, UNSET_MIDI_CODE);
    }
  }

  count = 0;
  offset = 0;
  for (i = 0; i < state_config.midi_counts.length; i++) {
    sprintf(name, "MIDI_%d_COUNT", i + 1);
    state_config.midi_counts.values[i] = config_file_get_int(config, name, 0);
    state_config.midi_offsets.values[i] = count;
    state_config.values_offsets.values[i] = offset;
    offset += state_config.midi_counts.values[i] *
              state_config.midi_active_counts.values[i];
    count += state_config.midi_counts.values[i];
  }

  state_config.midi_codes.length = count * 3;

  for (i = 0; i < state_config.midi_counts.length; i++) {
    offset = state_config.midi_offsets.values[i];
    for (j = 0; j < state_config.midi_counts.values[i]; j++) {
      sprintf(name, "MIDI_%d_%d_X", i + 1, j + 1);
      state_config.midi_codes.values[(offset + j) * 3] =
          config_file_get_int(config, name, UNSET_MIDI_CODE);

      sprintf(name, "MIDI_%d_%d_Y", i + 1, j + 1);
      state_config.midi_codes.values[(offset + j) * 3 + 1] =
          config_file_get_int(config, name, UNSET_MIDI_CODE);

      sprintf(name, "MIDI_%d_%d_Z", i + 1, j + 1);
      state_config.midi_codes.values[(offset + j) * 3 + 2] =
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

  if (context->state.values[context->selected] >= page_item_min &&
      context->state.values[context->selected] < page_item_max) {
    for (i = 0; i < state_config.select_item_codes.length; i++) {
      safe_midi_write(midi, state_config.select_item_codes.values[i],
                      i == context->state.values[context->selected] -
                                  page_item_min
                          ? MIDI_MAX
                          : 0);
    }
  } else {
    for (i = 0; i < state_config.select_item_codes.length; i++) {
      safe_midi_write(midi, state_config.select_item_codes.values[i], 0);
    }
  }
}

static void update_active(SharedContext *context, StateConfig state_config,
                          MidiDevice midi) {
  unsigned int i, j, k;

  for (i = 0; i < state_config.midi_active_counts.length; i++) {
    for (j = 0; j < state_config.midi_active_counts.values[i]; j++) {
      k = state_config.midi_active_offsets.values[i] + j;
      safe_midi_write(midi, state_config.midi_active_codes.values[k],
                      context->active[i] == j ? MIDI_MAX : 0);
    }
  }
}

static void update_values(SharedContext *context, StateConfig state_config,
                          MidiDevice midi) {
  unsigned int i, j, k, part;

  for (i = 0; i < state_config.midi_codes.length; i++) {
    j = i / 3;
    part = arr_uint_remap_index(state_config.midi_offsets, &j);
    k = state_config.values_offsets.values[part] +
        context->active[part] * state_config.midi_counts.values[part] + j;
    safe_midi_write(midi, state_config.midi_codes.values[i],
                    context->values[k][i % 3] * MIDI_MAX);
  }
}

void state_apply_event(SharedContext *context, StateConfig state_config,
                       MidiDevice midi, unsigned char code,
                       unsigned char value) {
  unsigned int i, j, k, part;
  bool found;

  found = false;

  // PAGE CHANGE
  i = arr_uint_index_of(state_config.select_page_codes, code);
  if (i != ARRAY_NOT_FOUND) {
    found = true;
    if (value > 0) {
      context->page = i;
      update_page(context, state_config, midi);
    }
  }

  // TARGET CHANGE
  i = arr_uint_index_of(state_config.select_frag_codes, code);
  if (i != ARRAY_NOT_FOUND) {
    found = true;
    if (value > 0) {
      context->selected = i;
      update_page(context, state_config, midi);
    }
  }

  // ITEM CHANGE
  i = arr_uint_index_of(state_config.select_item_codes, code);
  if (i != ARRAY_NOT_FOUND) {
    found = true;
    if (value > 0) {
      context->state.values[context->selected] =
          context->page * state_config.select_item_codes.length + i;
      update_page(context, state_config, midi);
    }
  }

  // ACTIVE CHANGE
  i = arr_uint_index_of(state_config.midi_active_codes, code);
  if (i != ARRAY_NOT_FOUND) {
    found = true;
    if (value > 0) {
      part = arr_uint_remap_index(state_config.midi_active_offsets, &i);
      context->active[part] = i;
      update_active(context, state_config, midi);
      update_values(context, state_config, midi);
    }
  }

  // VALUE CHANGE
  i = arr_uint_index_of(state_config.midi_codes, code);
  if (i != ARRAY_NOT_FOUND) {
    found = true;
    j = i / 3;
    part = arr_uint_remap_index(state_config.midi_offsets, &j);
    k = state_config.values_offsets.values[part] +
        context->active[part] * state_config.midi_counts.values[part] + j;

    if (arr_uint_index_of(state_config.fader_codes, code) != ARRAY_NOT_FOUND) {
      context->values[k][i % 3] = (float)value / MIDI_MAX;
    } else if (value > 0) {
      if (context->values[k][i % 3] > 0.5) {
        context->values[k][i % 3] = 0;
        midi_write(midi, code, 0);
      } else {
        context->values[k][i % 3] = 1;
        midi_write(midi, code, MIDI_MAX);
      }
    }
  }

  if (code == state_config.tap_tempo_code) {
    found = true;
    midi_write(midi, code, value);
    if (value > 0) {
      tempo_tap(&context->tempo);
    }
  }

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
  bool beat_active, last_active, change, last_change;

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
  update_active(context, state_config, midi);
  update_values(context, state_config, midi);

  last_active = false;
  last_change = false;

  while (!context->stop) {
    beat_active = tempo_progress(context->tempo, 1.0) < 0.25;

    if (beat_active != last_active) {
      safe_midi_write(midi, state_config.tap_tempo_code,
                      beat_active ? MIDI_MAX : 0);

      safe_midi_write(midi,
                      state_config.select_frag_codes.values[context->selected],
                      beat_active ? MIDI_MAX : 0);

      last_active = beat_active;
    }

    change = tempo_progress(context->tempo, 4.0) < 0.25;

    if (change && !last_change && context->demo) {
      state_randomize(context, state_config);
    }

    last_change = change;
  }

  log_info("(state) background writing stopped by main thread (pid: %d)", pid);
  return false;
}

void state_load(SharedContext *context, StateConfig state_config,
                char *state_file) {
  ConfigFile saved_state;
  char key[100];
  unsigned int i;

  saved_state = config_file_read(state_file, false);

  tempo_set(&context->tempo,
            config_file_get_int(saved_state, "tempo", context->tempo.tempo));
  context->page = config_file_get_int(saved_state, "page", 0);
  context->selected = config_file_get_int(saved_state, "selected", 0);

  for (i = 0; i < context->state.length; i++) {
    sprintf(key, "seed_%d", i);
    context->seeds[i] =
        config_file_get_int(saved_state, key, context->seeds[i]);
    sprintf(key, "state_%d", i);
    context->state.values[i] = config_file_get_int(saved_state, key, 0);
  }

  for (i = 0; i < state_config.midi_active_counts.length; i++) {
    sprintf(key, "active_%d", i);
    context->active[i] = config_file_get_int(saved_state, key, 0);
  }

  for (i = 0; i < state_config.midi_codes.length; i++) {
    sprintf(key, "value_%d_x", i);
    context->values[i][0] =
        (float)config_file_get_int(saved_state, key, 0) / MIDI_MAX;
    sprintf(key, "value_%d_y", i);
    context->values[i][1] =
        (float)config_file_get_int(saved_state, key, 0) / MIDI_MAX;
    sprintf(key, "value_%d_z", i);
    context->values[i][2] =
        (float)config_file_get_int(saved_state, key, 0) / MIDI_MAX;
  }

  config_file_free(saved_state);
}

void state_init(SharedContext *context, StateConfig state_config, bool demo,
                unsigned int base_tempo, char *state_file, bool load_state) {
  unsigned int i;

  context->tempo = tempo_init();
  tempo_set(&context->tempo, base_tempo);
  context->demo = demo;

  context->state.length = state_config.select_frag_codes.length;
  memset(context->state.values, 0, sizeof(context->state.values));

  if (demo) {
    state_randomize(context, state_config);
  }

  memset(context->active, 0, sizeof(context->active));
  memset(context->values, 0, sizeof(context->values));

  context->page = 0;
  context->selected = 0;

  memset(context->seeds, 0, sizeof(context->seeds));

  for (i = 0; i < context->state.length; i++) {
    context->seeds[i] = rand_uint(1000);
  }

  if (load_state) {
    state_load(context, state_config, state_file);
  }
}

void state_randomize(SharedContext *context, StateConfig state_config) {
  unsigned int i;

  for (i = 0; i < context->state.length; i++) {
    context->state.values[i] = rand_uint(state_config.state_max);
  }
}

void state_save(SharedContext *context, StateConfig state_config,
                char *state_file) {
  ConstStringArray lines;
  unsigned int i;

  log_info("Saving state to '%s'...", state_file);

  lines.length = 0;

  sprintf(lines.values[lines.length++], "tempo=%d",
          (unsigned int)context->tempo.tempo);
  sprintf(lines.values[lines.length++], "page=%d", context->page);
  sprintf(lines.values[lines.length++], "selected=%d", context->selected);

  for (i = 0; i < context->state.length; i++) {
    sprintf(lines.values[lines.length++], "seed_%d=%d", i, context->seeds[i]);
    sprintf(lines.values[lines.length++], "state_%d=%d", i,
            context->state.values[i]);
  }

  for (i = 0; i < state_config.midi_active_counts.length; i++) {
    sprintf(lines.values[lines.length++], "active_%d=%d", i,
            context->active[i]);
  }

  for (i = 0; i < state_config.midi_codes.length; i++) {
    sprintf(lines.values[lines.length++], "value_%d_x=%d", i,
            (unsigned int)(context->values[i][0] * MIDI_MAX));
    sprintf(lines.values[lines.length++], "value_%d_y=%d", i,
            (unsigned int)(context->values[i][1] * MIDI_MAX));
    sprintf(lines.values[lines.length++], "value_%d_z=%d", i,
            (unsigned int)(context->values[i][2] * MIDI_MAX));
  }

  file_write(state_file, lines);
}
