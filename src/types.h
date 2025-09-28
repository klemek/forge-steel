#include <GLFW/glfw3.h>
#include <alsa/asoundlib.h>
#include <glad/egl.h>
#include <glad/gl.h>
#include <hashmap.h>
#include <linmath.h>
#include <linux/videodev2.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#include "config.h"

#ifndef TYPES_H
#define TYPES_H

typedef struct Parameters {
  bool hot_reload;
  bool output;
  unsigned int output_screen;
  bool monitor;
  unsigned int monitor_screen;
  char *frag_path;
  char *config_path;
  unsigned int internal_size;
  unsigned int video_size;
  float base_tempo;
  bool demo;
  bool windowed;
  char *video_in[MAX_VIDEO];
  unsigned int video_in_count;
} Parameters;

typedef struct Vertex {
  vec2 pos;
} Vertex;

typedef struct File {
  char *path;
  char *content;
  bool error;
  time_t last_write;
} File;

typedef struct ShaderProgram {
  bool error;

  int last_width;
  int last_height;

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
  GLuint ifps_locations[ARRAY_SIZE];
  GLuint ires_locations[ARRAY_SIZE];
  GLuint itexres_locations[ARRAY_SIZE];
  GLuint iinres_locations[ARRAY_SIZE];
  GLuint iinfmt_locations[ARRAY_SIZE];
  GLuint iinfps_locations[ARRAY_SIZE];
  GLuint idemo_locations[ARRAY_SIZE];
  GLuint iseed_locations[ARRAY_SIZE];
  GLuint istate_locations[ARRAY_SIZE];
  GLuint ipage_locations[ARRAY_SIZE];
  GLuint iselected_locations[ARRAY_SIZE];
  GLuint iactive_locations[ARRAY_SIZE];

  GLuint vpos_locations[ARRAY_SIZE];

  GLuint textures_locations[ARRAY_SIZE];

  unsigned int sub_type_count;
  unsigned int sub_variant_count;
  GLuint sub_locations[ARRAY_SIZE];

  unsigned int active_count;

  unsigned int in_count;
  EGLDisplay egl_display;
} ShaderProgram;

typedef struct VideoCapture {
  char *name;
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

typedef GLFWwindow Window;

typedef struct SharedContext {
  int fd;

  int width;
  int height;
  unsigned int internal_width;
  unsigned int internal_height;
  double time;
  unsigned int fps;
  float tempo;
  unsigned int state[MAX_FRAG];
  unsigned int page;
  unsigned int selected;
  unsigned int active[ARRAY_SIZE];
  bool demo;
  unsigned int seeds[MAX_FRAG];
  bool monitor;
  unsigned int input_widths[MAX_VIDEO];
  unsigned int input_heights[MAX_VIDEO];
  unsigned int input_formats[MAX_VIDEO];
  unsigned int input_fps[MAX_VIDEO];
  bool stop;
} SharedContext;

typedef struct StateConfig {
  unsigned int select_page_count;
  unsigned int select_page_codes[ARRAY_SIZE];

  unsigned int select_item_count;
  unsigned int select_item_codes[ARRAY_SIZE];

  unsigned int select_frag_codes[ARRAY_SIZE];

  unsigned int src_count;
  unsigned int src_active_counts[ARRAY_SIZE];
  unsigned int src_active_offsets[ARRAY_SIZE];
  unsigned int src_active_codes[ARRAY_SIZE];
  unsigned int src_subcounts[ARRAY_SIZE];
  unsigned int src_offsets[ARRAY_SIZE];
  unsigned int src_codes[ARRAY_SIZE];

  unsigned int fader_count;
  unsigned int fader_codes[ARRAY_SIZE];

  unsigned int tap_tempo_code;
} StateConfig;

typedef struct Timer {
  struct timeval start;
  unsigned int counter;
  unsigned int target;
} Timer;

typedef struct ConfigFile {
  struct hashmap *map;
} ConfigFile;

typedef struct ConfigFileItem {
  char key[256];
  char value[2048];
} ConfigFileItem;

typedef struct MidiDevice {
  bool error;
  char *name;
  snd_rawmidi_t *input;
  snd_rawmidi_t *output;
} MidiDevice;

#endif /* TYPES_H */