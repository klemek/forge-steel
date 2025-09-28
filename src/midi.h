#include "types.h"

#ifndef MIDI_H
#define MIDI_H

MidiDevice midi_open(char *name);
void midi_close(MidiDevice device);

#endif /* MIDI_H */