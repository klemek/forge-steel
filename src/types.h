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

  GLuint programs[FRAMEBUFFER_COUNT + 1];

  GLuint vertex_shader;
  GLuint output_fragment_shader;

  GLuint fragment_shader; // TODO multiple

  GLuint itime_locations[FRAMEBUFFER_COUNT];
  GLuint ires_locations[FRAMEBUFFER_COUNT];
  GLuint frames_locations[FRAMEBUFFER_COUNT + 1][FRAMEBUFFER_COUNT];
  GLuint vpos_locations[FRAMEBUFFER_COUNT + 1];

  GLuint vertex_buffer;
  GLuint vertex_array;

  GLuint frame_buffers[FRAMEBUFFER_COUNT];
  GLuint textures[FRAMEBUFFER_COUNT];
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