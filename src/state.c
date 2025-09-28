#include <log.h>

#include "config.h"
#include "config_file.h"
#include "midi.h"
#include "rand.h"
#include "state.h"
#include "types.h"

StateConfig state_parse_config(ConfigFile config) {
  unsigned int i, j, offset, total, frag_count;
  StateConfig state_config;
  char name[256];

  state_config.select_page_count =
      config_file_get_int(config, "SELECT_PAGE_COUNT", 0);

  for (i = 0; i < state_config.select_page_count; i++) {
    sprintf(name, "SELECT_PAGE_%d", i + 1);
    state_config.select_page_codes[i] =
        config_file_get_int(config, name, UNSET_MIDI_CODE);
  }

  state_config.select_item_count =
      config_file_get_int(config, "SELECT_ITEM_COUNT", 0);

  for (i = 0; i < state_config.select_item_count; i++) {
    sprintf(name, "SELECT_ITEM_%d", i + 1);
    state_config.select_item_codes[i] =
        config_file_get_int(config, name, UNSET_MIDI_CODE);
  }

  frag_count = config_file_get_int(config, "FRAG_COUNT", 1);

  for (i = 0; i < frag_count; i++) {
    sprintf(name, "SELECT_FRAG_%d", i + 1);
    state_config.select_frag_codes[i] =
        config_file_get_int(config, name, UNSET_MIDI_CODE);
  }

  state_config.src_count = config_file_get_int(config, "SRC_COUNT", 0);

  total = 0;
  for (i = 0; i < state_config.src_count; i++) {
    sprintf(name, "SRC_%d_ACTIVE_COUNT", i + 1);
    state_config.src_active_counts[i] = config_file_get_int(config, name, 0);
    state_config.src_active_offsets[i] = total;
    total += state_config.src_active_counts[i];
  }

  for (i = 0; i < state_config.src_count; i++) {
    for (j = 0; j < state_config.src_active_counts[i]; j++) {
      sprintf(name, "SRC_%d_ACTIVE_%d", i + 1, j + 1);
      state_config.src_active_codes[state_config.src_active_offsets[i] + j] =
          config_file_get_int(config, name, UNSET_MIDI_CODE);
    }
  }

  total = 0;
  for (i = 0; i < state_config.src_count; i++) {
    sprintf(name, "SRC_%d_COUNT", i + 1);
    state_config.src_subcounts[i] = config_file_get_int(config, name, 0);
    state_config.src_offsets[i] = total;
    total += state_config.src_subcounts[i];
  }

  for (i = 0; i < state_config.src_count; i++) {
    offset = state_config.src_offsets[i];
    for (j = 0; j < state_config.src_subcounts[i]; j++) {
      sprintf(name, "SRC_%d_%d_X", i + 1, j + 1);
      state_config.src_active_codes[(offset + j) * 3] =
          config_file_get_int(config, name, UNSET_MIDI_CODE);

      sprintf(name, "SRC_%d_%d_Y", i + 1, j + 1);
      state_config.src_active_codes[(offset + j) * 3 + 1] =
          config_file_get_int(config, name, UNSET_MIDI_CODE);

      sprintf(name, "SRC_%d_%d_Z", i + 1, j + 1);
      state_config.src_active_codes[(offset + j) * 3 + 2] =
          config_file_get_int(config, name, UNSET_MIDI_CODE);
    }
  }

  state_config.fader_count = config_file_get_int(config, "FADER_COUNT", 0);

  for (i = 0; i < state_config.fader_count; i++) {
    sprintf(name, "FADER_%d", i + 1);
    state_config.fader_codes[i] =
        config_file_get_int(config, name, UNSET_MIDI_CODE);
  }

  state_config.tap_tempo_code =
      config_file_get_int(config, "TAP_TEMPO", UNSET_MIDI_CODE);

  return state_config;
}

void state_apply_event(SharedContext *context, StateConfig state_config,
                       unsigned int state_count, MidiDevice midi,
                       unsigned char code, float value) {
  log_debug("midi: %d %.2f", code, value);
  midi_write(midi, code, value);
  // TODO
}

bool state_background_midi_write(SharedContext *context,
                                 StateConfig state_config, MidiDevice midi) {
  pid_t pid;
  int bytes_read;
  unsigned char buffer[3];

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

void state_randomize(SharedContext *context, StateConfig state_config,
                     unsigned int state_count) {
  unsigned int i;

  for (i = 0; i < state_count; i++) {
    context->state[i] = rand_uint(state_config.select_page_count *
                                  state_config.select_item_count);
  }
}