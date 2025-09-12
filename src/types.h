#include "linmath.h"
#include <glad/gl.h>

#ifndef TYPES_H
#define TYPES_H

typedef struct Parameters {
  unsigned char screen;
} Parameters;

typedef struct Vertex {
  vec2 pos;
} Vertex;

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
} ShaderProgram;

#endif