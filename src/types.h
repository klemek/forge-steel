#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <hashmap.h>
#include <linmath.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#include "config.h"

#ifndef TYPES_H
#define TYPES_H

typedef struct Parameters {
  unsigned char screen;
  char *frag_path;
  char *frag_config_path;
  bool hot_reload;
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

  GLuint programs[FRAG_COUNT + 1];

  GLuint vertex_shader;
  GLuint output_shader;
  GLuint monitor_shader;

  GLuint fragment_shaders[FRAG_COUNT];

  GLuint itime_locations[FRAG_COUNT];
  GLuint itempo_locations[FRAG_COUNT];
  GLuint ifps_locations[FRAG_COUNT];
  GLuint ires_locations[FRAG_COUNT];

  GLuint textures_locations[FRAG_COUNT + 1][TEX_COUNT];

  GLuint sub_src_indexes[FRAG_COUNT][SUB_COUNT];
  GLuint sub_fx_indexes[FRAG_COUNT][SUB_COUNT];
  GLuint sub_mix_indexes[FRAG_COUNT][2];

  GLuint vpos_locations[FRAG_COUNT + 1];

  GLuint vertex_buffer;
  GLuint vertex_array;

  GLuint frame_buffers[FRAG_COUNT];
  GLuint textures[TEX_COUNT];

  GLenum draw_buffers[TEX_COUNT];
} ShaderProgram;

typedef GLFWwindow Window;

typedef struct Context {
  int width;
  int height;
  double time;
  unsigned int fps;
} Context;

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

#endif /* TYPES_H */