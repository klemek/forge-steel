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

  device.name = name;
  device.input = NULL;
  device.output = NULL;

  snd_rawmidi_open(&device.input, &device.output, name, SND_RAWMIDI_NONBLOCK);

  device.error = device.input == NULL || device.output == NULL;

  if (device.error) {
    midi_close(device);
  }

  log_debug("(%s) MIDI open", name);

  return device;
}

void midi_background_listen(MidiDevice device, SharedContext *context) {
  pid_t pid;
  int bytes_read;
  unsigned char buffer[3];

  pid = fork();
  if (pid < 0) {
    log_error("Could not create subprocess");
    return;
  }
  if (pid == 0) {
    return;
  }
  log_info("(%s) background acquisition started (pid: %d)", device.name, pid);

  while (!context->stop) {
    bytes_read = snd_rawmidi_read(device.input, buffer, 3);
    if (bytes_read == 3) {
      log_debug("midi: %d %.2f", buffer[1], (float)buffer[2] / 256);
    }
  }

  log_info("(%s) background acquisition stopped by main thread (pid: %d)",
           device.name, pid);
}

// int bytes_read = snd_rawmidi_read(input, input_buffer, sizeof(input_buffer));
// snd_rawmidi_write(output, buffer, 3);