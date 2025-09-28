#include "types.h"

#ifndef MIDI_H
#define MIDI_H

MidiDevice midi_open(char *name);
bool midi_background_listen(MidiDevice device, SharedContext *context);

#endif /* MIDI_H */