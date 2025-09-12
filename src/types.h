#include <stdbool.h>
#include <time.h>

#include <glad/gl.h>
#include <linmath.h>

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
  GLuint program;

  GLuint vertex_buffer;

  GLuint vertex_shader;
  GLuint fragment_shader;

  GLuint mvp_location;
  GLuint vpos_location;
  GLuint itime_location;
  GLuint ires_location;

  GLuint vertex_array;

  bool error;
} ShaderProgram;

#endif