#include <GLFW/glfw3.h>
#include <alsa/asoundlib.h>
#include <glad/egl.h>
#include <glad/gl.h>
#include <hashmap.h>
#include <linmath.h>
#ifdef VIDEO_IN
#include <linux/videodev2.h>
#endif /* VIDEO_IN */
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#include "config.h"

#ifndef TYPES_H
#define TYPES_H

#define ARRAY(X, Y)                                                            \
  struct X {                                                                   \
    Y values[ARRAY_SIZE];                                                      \
    unsigned int length;                                                       \
  } X

typedef ARRAY(UintArray, unsigned int);
typedef ARRAY(Vec3Array, vec3);
typedef ARRAY(GLuintArray, GLuint);

typedef struct StringArray {
  char values[ARRAY_SIZE][STR_LEN];
  unsigned int length;
} StringArray;

// args.c

typedef struct Parameters {
  char project_path[STR_LEN];
  char config_file[STR_LEN];
  bool hot_reload;
  bool output;
  unsigned int output_screen;
  bool monitor;
  unsigned int monitor_screen;
  bool windowed;
  float base_tempo;
  bool demo;
  bool auto_random;
  unsigned int auto_random_cycle;
  StringArray video_in;
  unsigned int video_size;
  unsigned int internal_size;
  bool load_state;
  bool save_state;
  bool trace_midi;
  bool trace_fps;
} Parameters;

// file.c

typedef struct File {
  char path[STR_LEN];
  char *content;
  unsigned int length;
  bool error;
  time_t last_write;
} File;

typedef ARRAY(FileArray, File);

// shaders.c

typedef struct Vertex {
  vec2 pos;
} Vertex;

typedef struct ShaderProgram {
  bool error;

  vec2 last_resolution;

  GLuint vertex_shader;

  GLuint vertex_buffer;
  GLuint vertex_array[2];

  unsigned int tex_count;
  GLuint textures[ARRAY_SIZE];

  unsigned int frag_count;
  unsigned int frag_output_index;
  unsigned int frag_monitor_index;

  GLuint programs[ARRAY_SIZE];

  GLuint frame_buffers[ARRAY_SIZE];
  GLuint fragment_shaders[ARRAY_SIZE];

  GLuint itime_locations[ARRAY_SIZE];
  GLuint itempo_locations[ARRAY_SIZE];
  GLuint ibeats_locations[ARRAY_SIZE];
  GLuint ifps_locations[ARRAY_SIZE];
  GLuint ires_locations[ARRAY_SIZE];
  GLuint itexres_locations[ARRAY_SIZE];
  GLuint iinres_locations[ARRAY_SIZE];
  GLuint iinfmt_locations[ARRAY_SIZE];
  GLuint iinfps_locations[ARRAY_SIZE];
  GLuint idemo_locations[ARRAY_SIZE];
  GLuint iautorand_locations[ARRAY_SIZE];
  GLuint iautorandcycle_locations[ARRAY_SIZE];
  GLuint iseed_locations[ARRAY_SIZE];
  GLuint istate_locations[ARRAY_SIZE];
  GLuint ipage_locations[ARRAY_SIZE];
  GLuint iselected_locations[ARRAY_SIZE];
  GLuint iactive_locations[ARRAY_SIZE];

  UintArray midi_lengths;
  GLuint imidi_locations[ARRAY_SIZE];

  GLuint vpos_locations[ARRAY_SIZE];

  GLuint textures_locations[ARRAY_SIZE];

  unsigned int sub_type_count;
  unsigned int sub_variant_count;
  GLuint sub_locations[ARRAY_SIZE];

  unsigned int active_count;

  unsigned int in_count;
  EGLDisplay egl_display;
} ShaderProgram;

// video.c

#ifndef VIDEO_IN
struct v4l2_buffer {};
#endif /* VIDEO_IN */

typedef struct VideoCapture {
  char name[STR_LEN];
  bool error;
  int fd;
  int exp_fd;
  unsigned int width;
  unsigned int height;
  unsigned int pixelformat;
  unsigned int bytesperline;
  bool output;
  struct v4l2_buffer buf;
  EGLImageKHR dma_image;
} VideoCapture;

typedef ARRAY(VideoCaptureArray, VideoCapture);

// window.c

typedef GLFWwindow Window;

// tempo.c

typedef struct Tempo {
  long last_reset;
  long last_tap;
  unsigned int taps_in_chain;
  unsigned int tap_duration_index;
  unsigned int tap_durations[MAX_TAP_VALUES];
  bool last_tap_skipped;
  long beat_length;
  float tempo;
} Tempo;

// context.c

typedef struct SharedContext {
  int fd;
  vec2 resolution;
  vec2 tex_resolution;
  vec2 input_resolutions[MAX_VIDEO];
  double time;
  Tempo tempo;
  double tempo_total;
  UintArray state;
  unsigned int page;
  unsigned int selected;
  unsigned int active[ARRAY_SIZE];
  vec3 values[ARRAY_SIZE];
  bool demo;
  bool auto_random;
  unsigned int auto_random_cycle;
  unsigned int seeds[MAX_FRAG];
  unsigned int fps;
  unsigned int input_formats[MAX_VIDEO];
  unsigned int input_fps[MAX_VIDEO];
  bool stop;
} SharedContext;

// state.c

typedef struct StateConfig {
  unsigned int state_max;

  UintArray select_page_codes;
  UintArray select_item_codes;
  UintArray select_frag_codes;
  UintArray midi_active_counts;
  UintArray midi_active_offsets;
  UintArray midi_active_codes;
  UintArray midi_counts;
  UintArray midi_offsets;
  UintArray midi_codes;
  UintArray fader_codes;
  UintArray values_offsets;

  unsigned int value_count;

  unsigned int tap_tempo_code;

  char save_file_prefix[STR_LEN];

  unsigned int hotkey_randomize;
  unsigned int hotkey_reset;
  unsigned int hotkey_demo;
  unsigned int hotkey_autorand;
  unsigned int hotkey_autorand_up;
  unsigned int hotkey_autorand_down;
  unsigned int hotkey_tempo_up;
  unsigned int hotkey_tempo_down;

  UintArray hotkey_load;
  UintArray hotkey_save;
} StateConfig;

// timer.c

typedef struct Timer {
  struct timeval start;
  unsigned int counter;
  unsigned int target;
} Timer;

// config.c

typedef struct ConfigFile {
  struct hashmap *map;
  bool error;
} ConfigFile;

typedef struct ConfigFileItem {
  char key[STR_LEN];
  char value[STR_LEN];
} ConfigFileItem;

// midi.c

typedef struct MidiDevice {
  bool error;
  char name[STR_LEN];
  snd_rawmidi_t *input;
  snd_rawmidi_t *output;
} MidiDevice;

// project.c

typedef struct Project {
  bool error;
  char path[STR_LEN];
  ConfigFile config;
  StateConfig state_config;
  unsigned int frag_count;
  unsigned int in_count;
  UintArray sub_counts;
  File fragment_shaders[MAX_FRAG][MAX_SUB_FILE + 1];
} Project;

#endif /* TYPES_H */