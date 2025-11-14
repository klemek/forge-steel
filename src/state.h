#include "types.h"

#ifndef STATE_H
#define STATE_H

void state_parse_config(StateConfig *state_config, const ConfigFile *config);

void state_midi_event(SharedContext *context, const StateConfig *state_config,
                      const MidiDevice *midi, unsigned char code,
                      unsigned char value, bool trace_midi);

void state_key_event(SharedContext *context, const StateConfig *state_config,
                     unsigned int code, const MidiDevice *midi);

bool state_background_write(SharedContext *context,
                            const StateConfig *state_config,
                            const MidiDevice *midi);

void state_init(SharedContext *context, const StateConfig *state_config,
                bool demo, bool auto_random, unsigned int auto_random_cycles,
                unsigned int base_tempo, const char *state_file,
                bool load_state);

void state_save(const SharedContext *context, const StateConfig *state_config,
                const char *state_file);

#endif /* STATE_H */