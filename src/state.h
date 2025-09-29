#include "types.h"

#ifndef STATE_H
#define STATE_H

StateConfig state_parse_config(ConfigFile config);

void state_apply_event(SharedContext *context, StateConfig state_config,
                       MidiDevice midi, unsigned char code, float value);

bool state_background_midi_write(SharedContext *context,
                                 StateConfig state_config, MidiDevice midi);

void state_randomize(SharedContext *context, StateConfig state_config);

#endif /* STATE_H */