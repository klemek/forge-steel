#include "types.h"

#ifndef MIDI_H
#define MIDI_H

MidiDevice midi_open(char *name);
void midi_write(MidiDevice device, unsigned char code, unsigned char value);
bool midi_background_listen(MidiDevice device, SharedContext *context,
                            void (*event_callback)(unsigned char code,
                                                   unsigned char value));

#endif /* MIDI_H */