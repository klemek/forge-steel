#include <log.h>
#include <stdio.h>

#include "types.h"

#include "arr.h"
#include "config.h"
#include "config_file.h"
#include "file.h"
#include "midi.h"
#include "rand.h"
#include "state.h"
#include "tempo.h"

static void safe_midi_write(const MidiDevice *midi, unsigned int code,
                            unsigned char value) {
  if (code != UNSET_MIDI_CODE) {
    midi_write(midi, code, value);
  }
}

static void update_page(const SharedContext *context,
                        const StateConfig *state_config,
                        const MidiDevice *midi) {
  unsigned int page_item_min;
  unsigned int page_item_max;
  // SHOW PAGE
  for (unsigned int i = 0; i < state_config->select_page_codes.length; i++) {
    safe_midi_write(midi, state_config->select_page_codes.values[i],
                    i == context->page ? MIDI_MAX : 0);
  }

  // SHOW PAGE ITEM
  page_item_min = state_config->select_item_codes.length * context->page;
  page_item_max = page_item_min + state_config->select_item_codes.length;

  if (context->state.values[context->selected] >= page_item_min &&
      context->state.values[context->selected] < page_item_max) {
    for (unsigned int i = 0; i < state_config->select_item_codes.length; i++) {
      safe_midi_write(midi, state_config->select_item_codes.values[i],
                      i == context->state.values[context->selected] -
                                  page_item_min
                          ? MIDI_MAX
                          : 0);
    }
  } else {
    for (unsigned int i = 0; i < state_config->select_item_codes.length; i++) {
      safe_midi_write(midi, state_config->select_item_codes.values[i], 0);
    }
  }
}

static void update_active(const SharedContext *context,
                          const StateConfig *state_config,
                          const MidiDevice *midi) {
  unsigned int k;

  for (unsigned int i = 0; i < state_config->midi_active_counts.length; i++) {
    for (unsigned int j = 0; j < state_config->midi_active_counts.values[i];
         j++) {
      k = state_config->midi_active_offsets.values[i] + j;
      safe_midi_write(midi, state_config->midi_active_codes.values[k],
                      context->active[i] == j ? MIDI_MAX : 0);
    }
  }
}

static void update_values(const SharedContext *context,
                          const StateConfig *state_config,
                          const MidiDevice *midi) {
  unsigned int j;
  unsigned int k;
  unsigned int part;

  for (unsigned int i = 0; i < state_config->midi_codes.length; i++) {
    j = i / 3;
    part = arr_uint_remap_index(state_config->midi_offsets, &j);
    k = state_config->values_offsets.values[part] +
        context->active[part] * state_config->midi_counts.values[part] + j;
    safe_midi_write(midi, state_config->midi_codes.values[i],
                    context->values[k][i % 3] * MIDI_MAX);
  }
}

static void reset(SharedContext *context) {
  memset(context->values, 0, sizeof(context->values));
  memset(context->state.values, 0, sizeof(context->state.values));
}

static void randomize(SharedContext *context, const StateConfig *state_config) {
  unsigned int j;
  unsigned int l;
  unsigned int part;

  for (unsigned int i = 0; i < state_config->midi_codes.length; i++) {
    j = i / 3;
    part = arr_uint_remap_index(state_config->midi_offsets, &j);
    for (unsigned int k = 0; k < state_config->midi_active_counts.values[part];
         k++) {
      l = state_config->values_offsets.values[part] +
          k * state_config->midi_counts.values[part] + j;

      if (arr_uint_index_of(state_config->fader_codes,
                            state_config->midi_codes.values[i]) !=
          ARRAY_NOT_FOUND) {
        context->values[l][i % 3] = (float)rand_uint(MIDI_MAX + 1) / MIDI_MAX;
      } else {
        context->values[l][i % 3] = rand_uint(2) == 1 ? 1 : 0;
      }
    }
  }

  for (unsigned int i = 0; i < context->state.length; i++) {
    context->state.values[i] = rand_uint(state_config->state_max);
  }
}

static void load_from_file(SharedContext *context,
                           const StateConfig *state_config,
                           const char *state_file) {
  ConfigFile saved_state;
  char key[STR_LEN];

  config_file_read(&saved_state, state_file);

  if (saved_state.error) {
    return;
  }

  tempo_set(&context->tempo,
            config_file_get_int(&saved_state, "tempo", context->tempo.tempo));
  context->page = config_file_get_int(&saved_state, "page", 0);
  context->selected = config_file_get_int(&saved_state, "selected", 0);

  for (unsigned int i = 0; i < context->state.length; i++) {
    snprintf(key, STR_LEN, "seed_%d", i);
    context->seeds[i] =
        config_file_get_int(&saved_state, key, context->seeds[i]);
    snprintf(key, STR_LEN, "state_%d", i);
    context->state.values[i] = config_file_get_int(&saved_state, key, 0);
  }

  for (unsigned int i = 0; i < state_config->midi_active_counts.length; i++) {
    snprintf(key, STR_LEN, "active_%d", i);
    context->active[i] = config_file_get_int(&saved_state, key, 0);
  }

  for (unsigned int i = 0; i < state_config->value_count; i++) {
    snprintf(key, STR_LEN, "value_%d_x", i);
    context->values[i][0] =
        (float)config_file_get_int(&saved_state, key, 0) / MIDI_MAX;
    snprintf(key, STR_LEN, "value_%d_y", i);
    context->values[i][1] =
        (float)config_file_get_int(&saved_state, key, 0) / MIDI_MAX;
    snprintf(key, STR_LEN, "value_%d_z", i);
    context->values[i][2] =
        (float)config_file_get_int(&saved_state, key, 0) / MIDI_MAX;
  }

  config_file_free(&saved_state);
}

static void load_from_default_file(SharedContext *context,
                                   const StateConfig *state_config) {
  char state_file[STR_LEN];

  snprintf(state_file, STR_LEN, "%s.txt", state_config->save_file_prefix);

  load_from_file(context, state_config, state_file);
}

static void load_from_index_file(SharedContext *context,
                                 const StateConfig *state_config,
                                 unsigned int index) {
  char state_file[STR_LEN];

  snprintf(state_file, STR_LEN, "%s.%d.txt", state_config->save_file_prefix,
           index);

  load_from_file(context, state_config, state_file);
}

static void save_to_file(const SharedContext *context,
                         const StateConfig *state_config,
                         const char *state_file) {
  StringArray lines;

  log_info("Saving state to '%s'...", state_file);

  lines.length = 0;

  snprintf(lines.values[lines.length++], STR_LEN, "tempo=%d",
           (unsigned int)context->tempo.tempo);
  snprintf(lines.values[lines.length++], STR_LEN, "page=%d", context->page);
  snprintf(lines.values[lines.length++], STR_LEN, "selected=%d",
           context->selected);

  for (unsigned int i = 0; i < context->state.length; i++) {
    snprintf(lines.values[lines.length++], STR_LEN, "seed_%d=%d", i,
             context->seeds[i]);
    snprintf(lines.values[lines.length++], STR_LEN, "state_%d=%d", i,
             context->state.values[i]);
  }

  for (unsigned int i = 0; i < state_config->midi_active_counts.length; i++) {
    snprintf(lines.values[lines.length++], STR_LEN, "active_%d=%d", i,
             context->active[i]);
  }

  for (unsigned int i = 0; i < state_config->value_count; i++) {
    snprintf(lines.values[lines.length++], STR_LEN, "value_%d_x=%d", i,
             (unsigned int)(context->values[i][0] * MIDI_MAX));
    snprintf(lines.values[lines.length++], STR_LEN, "value_%d_y=%d", i,
             (unsigned int)(context->values[i][1] * MIDI_MAX));
    snprintf(lines.values[lines.length++], STR_LEN, "value_%d_z=%d", i,
             (unsigned int)(context->values[i][2] * MIDI_MAX));
  }

  file_write(state_file, &lines);
}

static void save_to_default_file(const SharedContext *context,
                                 const StateConfig *state_config) {
  char state_file[STR_LEN];

  snprintf(state_file, STR_LEN, "%s.txt", state_config->save_file_prefix);

  save_to_file(context, state_config, state_file);
}

static void save_to_index_file(const SharedContext *context,
                               const StateConfig *state_config,
                               unsigned int index) {
  char state_file[STR_LEN];

  snprintf(state_file, STR_LEN, "%s.%d.txt", state_config->save_file_prefix,
           index);

  save_to_file(context, state_config, state_file);
}

void state_parse_config(StateConfig *state_config, const ConfigFile *config) {
  unsigned int offset;
  unsigned int count;
  char name[STR_LEN];

  state_config->select_page_codes.length =
      config_file_get_int(config, "SELECT_PAGE_COUNT", 0);

  for (unsigned int i = 0; i < state_config->select_page_codes.length; i++) {
    snprintf(name, STR_LEN, "SELECT_PAGE_%d", i + 1);
    state_config->select_page_codes.values[i] =
        config_file_get_int(config, name, UNSET_MIDI_CODE);
  }

  state_config->select_item_codes.length =
      config_file_get_int(config, "SELECT_ITEM_COUNT", 0);

  for (unsigned int i = 0; i < state_config->select_item_codes.length; i++) {
    snprintf(name, STR_LEN, "SELECT_ITEM_%d", i + 1);
    state_config->select_item_codes.values[i] =
        config_file_get_int(config, name, UNSET_MIDI_CODE);
  }

  state_config->state_max = state_config->select_page_codes.length *
                            state_config->select_item_codes.length;

  state_config->select_frag_codes.length =
      config_file_get_int(config, "FRAG_COUNT", 1);

  for (unsigned int i = 0; i < state_config->select_frag_codes.length; i++) {
    snprintf(name, STR_LEN, "SELECT_FRAG_%d", i + 1);
    state_config->select_frag_codes.values[i] =
        config_file_get_int(config, name, UNSET_MIDI_CODE);
  }

  state_config->midi_active_counts.length = state_config->midi_counts.length =
      state_config->midi_active_offsets.length =
          state_config->midi_offsets.length =
              state_config->values_offsets.length =
                  config_file_get_int(config, "MIDI_COUNT", 0);

  count = 0;
  for (unsigned int i = 0; i < state_config->midi_active_counts.length; i++) {
    snprintf(name, STR_LEN, "MIDI_%d_ACTIVE_COUNT", i + 1);
    state_config->midi_active_counts.values[i] =
        config_file_get_int(config, name, 1);
    state_config->midi_active_offsets.values[i] = count;
    count += state_config->midi_active_counts.values[i];
  }

  state_config->midi_active_codes.length = count;

  for (unsigned int i = 0; i < state_config->midi_active_counts.length; i++) {
    for (unsigned int j = 0; j < state_config->midi_active_counts.values[i];
         j++) {
      snprintf(name, STR_LEN, "MIDI_%d_ACTIVE_%d", i + 1, j + 1);
      state_config->midi_active_codes
          .values[state_config->midi_active_offsets.values[i] + j] =
          config_file_get_int(config, name, UNSET_MIDI_CODE);
    }
  }

  count = 0;
  offset = 0;
  for (unsigned int i = 0; i < state_config->midi_counts.length; i++) {
    snprintf(name, STR_LEN, "MIDI_%d_COUNT", i + 1);
    state_config->midi_counts.values[i] = config_file_get_int(config, name, 0);
    state_config->midi_offsets.values[i] = count;
    state_config->values_offsets.values[i] = offset;
    offset += state_config->midi_counts.values[i] *
              state_config->midi_active_counts.values[i];
    count += state_config->midi_counts.values[i];
  }

  state_config->value_count = offset;

  state_config->midi_codes.length = count * 3;

  for (unsigned int i = 0; i < state_config->midi_counts.length; i++) {
    offset = state_config->midi_offsets.values[i];
    for (unsigned int j = 0; j < state_config->midi_counts.values[i]; j++) {
      snprintf(name, STR_LEN, "MIDI_%d_%d_X", i + 1, j + 1);
      state_config->midi_codes.values[(offset + j) * 3] =
          config_file_get_int(config, name, UNSET_MIDI_CODE);

      snprintf(name, STR_LEN, "MIDI_%d_%d_Y", i + 1, j + 1);
      state_config->midi_codes.values[(offset + j) * 3 + 1] =
          config_file_get_int(config, name, UNSET_MIDI_CODE);

      snprintf(name, STR_LEN, "MIDI_%d_%d_Z", i + 1, j + 1);
      state_config->midi_codes.values[(offset + j) * 3 + 2] =
          config_file_get_int(config, name, UNSET_MIDI_CODE);
    }
  }

  state_config->fader_codes.length =
      config_file_get_int(config, "FADER_COUNT", 0);

  for (unsigned int i = 0; i < state_config->fader_codes.length; i++) {
    snprintf(name, STR_LEN, "FADER_%d", i + 1);
    state_config->fader_codes.values[i] =
        config_file_get_int(config, name, UNSET_MIDI_CODE);
  }

  state_config->tap_tempo_code =
      config_file_get_int(config, "TAP_TEMPO", UNSET_MIDI_CODE);

  strlcpy(state_config->save_file_prefix,
          config_file_get_str(config, "SAVE_FILE_PREFIX", "forge_save"),
          STR_LEN);

  state_config->hotkey_randomize =
      config_file_get_int(config, "HOTKEY_RANDOMIZE", 82);
  state_config->hotkey_reset =
      config_file_get_int(config, "HOTKEY_RESET", 1082);
  state_config->hotkey_demo = config_file_get_int(config, "HOTKEY_DEMO", 68);
  state_config->hotkey_autorand =
      config_file_get_int(config, "HOTKEY_AUTORAND", 65);
  state_config->hotkey_autorand_down =
      config_file_get_int(config, "HOTKEY_AUTORAND_DOWN", 263);
  state_config->hotkey_autorand_up =
      config_file_get_int(config, "HOTKEY_AUTORAND_UP", 262);
  state_config->hotkey_tempo_down =
      config_file_get_int(config, "HOTKEY_TEMPO_DOWN", 264);
  state_config->hotkey_tempo_up =
      config_file_get_int(config, "HOTKEY_TEMPO_UP", 265);

  if (config_file_has(config, "HOTKEY_LOAD_COUNT")) {
    state_config->hotkey_load.length =
        config_file_get_int(config, "HOTKEY_LOAD_COUNT", 0);

    for (unsigned int i = 0; i < state_config->hotkey_load.length; i++) {
      snprintf(name, STR_LEN, "HOTKEY_LOAD_%d", i + 1);
      state_config->hotkey_load.values[i] =
          config_file_get_int(config, name, 0);
    }
  } else {
    state_config->hotkey_load.length = 10;
    state_config->hotkey_load.values[0] = 49;
    state_config->hotkey_load.values[1] = 50;
    state_config->hotkey_load.values[2] = 51;
    state_config->hotkey_load.values[3] = 52;
    state_config->hotkey_load.values[4] = 53;
    state_config->hotkey_load.values[5] = 54;
    state_config->hotkey_load.values[6] = 55;
    state_config->hotkey_load.values[7] = 56;
    state_config->hotkey_load.values[8] = 57;
    state_config->hotkey_load.values[9] = 48;
  }

  if (config_file_has(config, "HOTKEY_SAVE_COUNT")) {
    state_config->hotkey_save.length =
        config_file_get_int(config, "HOTKEY_SAVE_COUNT", 0);

    for (unsigned int i = 0; i < state_config->hotkey_save.length; i++) {
      snprintf(name, STR_LEN, "HOTKEY_SAVE_%d", i + 1);
      state_config->hotkey_save.values[i] =
          config_file_get_int(config, name, 0);
    }
  } else {
    state_config->hotkey_save.length = 10;
    state_config->hotkey_save.values[0] = 1049;
    state_config->hotkey_save.values[1] = 1050;
    state_config->hotkey_save.values[2] = 1051;
    state_config->hotkey_save.values[3] = 1052;
    state_config->hotkey_save.values[4] = 1053;
    state_config->hotkey_save.values[5] = 1054;
    state_config->hotkey_save.values[6] = 1055;
    state_config->hotkey_save.values[7] = 1056;
    state_config->hotkey_save.values[8] = 1057;
    state_config->hotkey_save.values[9] = 1048;
  }
}

void state_midi_event(SharedContext *context, const StateConfig *state_config,
                      const MidiDevice *midi, unsigned char code,
                      unsigned char value, bool trace_midi) {
  unsigned int i;
  unsigned int j;
  unsigned int k;
  unsigned int part;
  bool found;

  found = false;

  // PAGE CHANGE
  i = arr_uint_index_of(state_config->select_page_codes, code);
  if (i != ARRAY_NOT_FOUND) {
    found = true;
    if (value > 0) {
      context->page = i;
      update_page(context, state_config, midi);
    }
  }

  // TARGET CHANGE
  i = arr_uint_index_of(state_config->select_frag_codes, code);
  if (i != ARRAY_NOT_FOUND) {
    found = true;
    if (value > 0) {
      context->selected = i;
      update_page(context, state_config, midi);
    }
  }

  // ITEM CHANGE
  i = arr_uint_index_of(state_config->select_item_codes, code);
  if (i != ARRAY_NOT_FOUND) {
    found = true;
    if (value > 0) {
      context->state.values[context->selected] =
          context->page * state_config->select_item_codes.length + i;
      update_page(context, state_config, midi);
    }
  }

  // ACTIVE CHANGE
  i = arr_uint_index_of(state_config->midi_active_codes, code);
  if (i != ARRAY_NOT_FOUND) {
    found = true;
    if (value > 0) {
      part = arr_uint_remap_index(state_config->midi_active_offsets, &i);
      context->active[part] = i;
      update_active(context, state_config, midi);
      update_values(context, state_config, midi);
    }
  }

  // VALUE CHANGE
  i = arr_uint_index_of(state_config->midi_codes, code);
  if (i != ARRAY_NOT_FOUND) {
    found = true;
    j = i / 3;
    part = arr_uint_remap_index(state_config->midi_offsets, &j);
    k = state_config->values_offsets.values[part] +
        context->active[part] * state_config->midi_counts.values[part] + j;

    if (arr_uint_index_of(state_config->fader_codes, code) != ARRAY_NOT_FOUND) {
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

  if (code == state_config->tap_tempo_code) {
    found = true;
    midi_write(midi, code, value);
    if (value > 0) {
      tempo_tap(&context->tempo);
    }
  }

  if (!found) {
    if (trace_midi) {
      log_trace("unknown midi: %d %d", code, value);
    }
    midi_write(midi, code, value);
  } else if (trace_midi) {
    log_trace("midi: %d %d", code, value);
  }
}

void state_key_event(SharedContext *context, const StateConfig *state_config,
                     unsigned int code, const MidiDevice *midi) {
  unsigned int index;

  if (code == state_config->hotkey_randomize) {
    log_info("[%d] Randomized", code);
    randomize(context, state_config);
    update_values(context, state_config, midi);
  } else if (code == state_config->hotkey_reset) {
    log_info("[%d] Reset", code);
    reset(context);
    update_values(context, state_config, midi);
  } else if (code == state_config->hotkey_demo) {
    log_info((context->demo ? "[%d] Demo OFF" : "[%d] Demo ON"), code);
    context->demo = !context->demo;
  } else if (code == state_config->hotkey_autorand) {
    log_info(
        (context->auto_random ? "[%d] Auto Random OFF" : "[%d] Auto Random ON"),
        code);
    context->auto_random = !context->auto_random;
  } else if (code == state_config->hotkey_autorand_down) {
    if (context->auto_random_cycle > 1) {
      context->auto_random_cycle -= 1;
    }
    log_info("[%d] Auto Random Cycle: %d", code, context->auto_random_cycle);
  } else if (code == state_config->hotkey_autorand_up) {
    context->auto_random_cycle += 1;
    log_info("[%d] Auto Random Cycle: %d", code, context->auto_random_cycle);
  } else if (code == state_config->hotkey_tempo_up) {
    tempo_set(&context->tempo, context->tempo.tempo + 1);
    log_info("[%d] Tempo: %f", code, context->tempo);
  } else if (code == state_config->hotkey_tempo_down) {
    if (context->tempo.tempo > 0) {
      tempo_set(&context->tempo, context->tempo.tempo - 1);
    }
    log_info("[%d] Tempo: %f", code, context->tempo);
  } else {
    index = arr_uint_index_of(state_config->hotkey_load, code);

    if (index != ARRAY_NOT_FOUND) {
      log_info("[%d] Loading state %d", code, index + 1);
      load_from_index_file(context, state_config, index + 1);
      return;
    }

    index = arr_uint_index_of(state_config->hotkey_save, code);

    if (index != ARRAY_NOT_FOUND) {
      log_info("[%d] Saving state %d", code, index + 1);
      save_to_index_file(context, state_config, index + 1);
      return;
    }

    log_info("[%d] No hotkey defined", code);
  }
}

bool state_background_write(SharedContext *context,
                            const StateConfig *state_config,
                            const MidiDevice *midi) {
  pid_t pid;
  bool beat_active;
  bool last_active;
  bool change;
  bool last_change;

  pid = fork();
  if (pid < 0) {
    log_error("Could not create subprocess");
    return false;
  }
  if (pid == 0) {
    return true;
  }
  log_info("(state) background writing started (pid: %d)", pid);

  if (!midi->error) {
    update_page(context, state_config, midi);
    update_active(context, state_config, midi);
    update_values(context, state_config, midi);
  }

  last_active = false;
  last_change = false;

  while (!context->stop) {
    beat_active = tempo_progress(&context->tempo, 1.0) < 0.5;

    if (!midi->error && beat_active != last_active) {
      safe_midi_write(midi, state_config->tap_tempo_code,
                      beat_active ? MIDI_MAX : 0);

      safe_midi_write(midi,
                      state_config->select_frag_codes.values[context->selected],
                      beat_active ? MIDI_MAX : 0);
    }

    last_active = beat_active;

    change = tempo_progress(&context->tempo,
                            (double)context->auto_random_cycle) < 0.5;

    if (context->auto_random && change && !last_change) {
      randomize(context, state_config);

      update_values(context, state_config, midi);
    }

    last_change = change;
  }

  log_info("(state) background writing stopped by main thread (pid: %d)", pid);
  return false;
}

void state_init(SharedContext *context, const StateConfig *state_config,
                bool demo, bool auto_random, unsigned int auto_random_cycles,
                unsigned int base_tempo, bool load_state) {
  tempo_init(&context->tempo, base_tempo);
  context->demo = demo;
  context->auto_random = auto_random;
  context->auto_random_cycle = auto_random_cycles;

  context->state.length = state_config->select_frag_codes.length;
  memset(context->state.values, 0, sizeof(context->state.values));

  if (auto_random) {
    randomize(context, state_config);
  }

  memset(context->active, 0, sizeof(context->active));
  memset(context->values, 0, sizeof(context->values));

  context->page = 0;
  context->selected = 0;

  memset(context->seeds, 0, sizeof(context->seeds));

  for (unsigned int i = 0; i < context->state.length; i++) {
    context->seeds[i] = rand_uint(1000);
  }

  if (load_state) {
    load_from_default_file(context, state_config);
  }
}

void state_save(const SharedContext *context, const StateConfig *state_config) {
  save_to_default_file(context, state_config);
}
