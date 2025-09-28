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

typedef struct SharedUint {
  int fd;
  unsigned int value;
} SharedUint;

typedef struct SharedBool {
  int fd;
  bool value;
} SharedBool;

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
  GLuint *textures;

  unsigned int frag_count;
  unsigned int frag_output_index;
  unsigned int frag_monitor_index;

  GLuint *programs;

  GLuint *frame_buffers;
  GLuint *fragment_shaders;

  GLuint *itime_locations;
  GLuint *itempo_locations;
  GLuint *ifps_locations;
  GLuint *ires_locations;
  GLuint *itexres_locations;
  GLuint *iinres_locations;
  GLuint *iinfmt_locations;
  GLuint *iinfps_locations;
  GLuint *idemo_locations;
  GLuint *iseed_locations;
  GLuint *istate_locations;

  GLuint *vpos_locations;

  GLuint *textures_locations;

  unsigned int sub_type_count;
  unsigned int sub_variant_count;
  GLuint *sub_locations;

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
  unsigned int *select_page_codes;

  unsigned int select_item_count;
  unsigned int *select_item_codes;

  unsigned int *select_frag_codes;

  unsigned int src_count;
  unsigned int *src_active_counts;
  unsigned int *src_active;
  unsigned int *src_subcounts;
  unsigned int *src_codes;

  unsigned int fader_count;
  unsigned int *fader_codes;

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