#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <linmath.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

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

  GLuint program;

  GLuint vertex_shader;
  GLuint fragment_shader;

  GLuint mvp_location;
  GLuint itime_location;
  GLuint ires_location;

  GLuint vertex_buffer;
  GLuint vertex_array;
  GLuint vpos_location;
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