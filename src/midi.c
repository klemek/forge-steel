#include <GLFW/glfw3.h>
#include <alsa/asoundlib.h>

#include "log.h"
#include "types.h"

void midi_close(MidiDevice device) {
  if (device.input != NULL) {
    snd_rawmidi_close(device.input);
  }
  if (device.output != NULL) {
    snd_rawmidi_close(device.input);
  }
}

MidiDevice midi_open(char *name) {
  MidiDevice device;

  device.input = NULL;
  device.output = NULL;

  snd_rawmidi_open(&device.input, &device.output, name, 0);

  device.error = device.input == NULL || device.output == NULL;

  if (device.error) {
    midi_close(device);
  }

  log_debug("(%s) MIDI open", name);

  return device;
}

// int bytes_read = snd_rawmidi_read(input, input_buffer, sizeof(input_buffer));
// snd_rawmidi_write(output, buffer, 3);