#include "state.h"
#include "config.h"
#include "config_file.h"
#include "rand.h"
#include "types.h"

StateConfig state_parse_config(ConfigFile config) {
  StateConfig state_config;

  state_config.select_page_count =
      config_file_get_int(config, "SELECT_PAGE_COUNT", 0);

  // TODO parse

  state_config.select_item_count =
      config_file_get_int(config, "SELECT_ITEM_COUNT", 0);

  // TODO parse

  state_config.src_count = config_file_get_int(config, "SRC_COUNT", 0);

  // TODO parse

  state_config.fader_count = config_file_get_int(config, "FADER_COUNT", 0);

  // TODO parse

  state_config.tap_tempo_code =
      config_file_get_int(config, "TAP_TEMPO", UNSET_MIDI_CODE);

  return state_config;
}

void state_free_config(StateConfig state_config) {
  // TODO
}

void state_randomize(SharedContext *context, StateConfig state_config,
                     unsigned int state_count) {
  unsigned int i;

  for (i = 0; i < state_count; i++) {
    context->state[i] =
        rand_uint(state_config.select_page_count *
                  state_config.select_item_count); // TODO from config
  }
}