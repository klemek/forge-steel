#include "types.h"

#ifndef STATE_H
#define STATE_H

StateConfig state_parse_config(ConfigFile config);

void state_apply_event(SharedContext *context, StateConfig state_config,
                       MidiDevice midi, unsigned char code, unsigned char value,
                       bool trace_midi);

bool state_background_write(SharedContext *context, StateConfig state_config,
                            MidiDevice midi);

void state_init(SharedContext *context, StateConfig state_config, bool demo,
                bool auto_random, unsigned int base_tempo, char *state_file,
                bool load_state);

void state_randomize(SharedContext *context, StateConfig state_config);

void state_save(SharedContext *context, StateConfig state_config,
                char *state_file);

#endif /* STATE_H */