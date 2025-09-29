#include <GLFW/glfw3.h>
#include <alsa/asoundlib.h>

#include "log.h"
#include "types.h"

MidiDevice midi_open(char *name) {
  MidiDevice device;

  device.name = name;
  device.input = NULL;
  device.output = NULL;

  snd_rawmidi_open(&device.input, &device.output, name, SND_RAWMIDI_NONBLOCK);

  device.error = device.input == NULL || device.output == NULL;

  log_info("(%s) MIDI open", name);

  return device;
}

void midi_write(MidiDevice device, unsigned char code, float value) {
  unsigned char buffer[3];

  buffer[0] = 0xB0;
  buffer[1] = code;
  buffer[2] = (unsigned char)(256 * value);

  snd_rawmidi_write(device.output, buffer, 3);
}

bool midi_background_listen(MidiDevice device, SharedContext *context,
                            void (*event_callback)(unsigned char code,
                                                   float value)) {
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
  log_info("(%s) background acquisition started (pid: %d)", device.name, pid);

  while (!context->stop) {
    bytes_read = snd_rawmidi_read(device.input, buffer, 3);
    if (bytes_read == 3) {
      event_callback(buffer[1], (float)buffer[2] / 256.0);
    }
  }

  log_info("(%s) background acquisition stopped by main thread (pid: %d)",
           device.name, pid);
  return false;
}
