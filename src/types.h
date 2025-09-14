#include <GLFW/glfw3.h>
#include <glad/gl.h>
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

  GLuint programs[BUFFER_COUNT + 1];

  GLuint vertex_shader;
  GLuint output_fragment_shader;

  GLuint fragment_shader; // TODO multiple

  GLuint itime_locations[BUFFER_COUNT];
  GLuint ires_locations[BUFFER_COUNT];
  GLuint frames_locations[BUFFER_COUNT + 1][BUFFER_COUNT];
  GLuint vpos_locations[BUFFER_COUNT + 1];

  GLuint vertex_buffer;
  GLuint vertex_array;

  GLuint frame_buffers[BUFFER_COUNT];
  GLuint textures[BUFFER_COUNT];
} ShaderProgram;

typedef GLFWwindow Window;

typedef struct Context {
  int width;
  int height;
  double time;
} Context;

typedef struct Timer {
  struct timeval start;
  unsigned int counter;
  unsigned int target;
} Timer;

#endif