#include <glad/gl.h>
#include <linmath.h>
#include <stddef.h>
#include <stdio.h>

#include "constants.h"
#include "types.h"

bool compile_shader(GLuint shader_id, char *name, char *source_code) {
  GLint status_params;
  char log[1024];

  // update shader source code
  glShaderSource(shader_id, 1, (const GLchar **)&source_code, NULL);

  // compile shader
  glCompileShader(shader_id);

  // get compilation status
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status_params);
  glGetShaderInfoLog(shader_id, 1024, NULL, (GLchar *)&log);

  if (status_params == GL_FALSE) {
    fprintf(stderr, "Failed to compile shader '%s'\n%s\n", name, log);
  } else {
    fprintf(stdout, "Compiled shader '%s'\n", name);
  }

  return status_params == GL_TRUE;
}

ShaderProgram init_program(File fragment_shader) {
  ShaderProgram program = {0};

  // create vertex buffer and setup vertices
  glGenBuffers(1, &program.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, program.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // compile vertex shader
  program.vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  program.error |= !compile_shader(
      program.vertex_shader, "internal vertex shader", vertex_shader_text);

  // compile fragment shader
  program.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  program.error |= !compile_shader(
      program.fragment_shader, fragment_shader.path, fragment_shader.content);

  if (program.error) {
    return program;
  }

  // create and link full shader program
  program.program = glCreateProgram();
  glAttachShader(program.program, program.vertex_shader);
  glAttachShader(program.program, program.fragment_shader);
  glLinkProgram(program.program);

  // create uniforms pointers
  program.mvp_location = glGetUniformLocation(program.program, "mvp");
  program.itime_location = glGetUniformLocation(program.program, "iTime");
  program.ires_location = glGetUniformLocation(program.program, "iResolution");

  // create attribute pointer
  program.vpos_location = glGetAttribLocation(program.program, "vPos");

  // create vertex array and bind to vPos attribute
  glGenVertexArrays(1, &program.vertex_array);
  glBindVertexArray(program.vertex_array);
  glEnableVertexAttribArray(program.vpos_location);
  glVertexAttribPointer(program.vpos_location, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void *)offsetof(Vertex, pos));

  return program;
}

void update_program(ShaderProgram program, File fragment_shader) {
  bool result;

  result = compile_shader(program.fragment_shader, fragment_shader.path,
                          fragment_shader.content);

  if (result) {
    // re-link program
    glLinkProgram(program.program);
  }
}

void apply_program(ShaderProgram program, Context context) {
  mat4x4 m, p, mvp;

  // update viewport
  glViewport(0, 0, context.width, context.height);
  // clear buffer
  glClear(GL_COLOR_BUFFER_BIT);

  // create model-view-projection matrix
  mat4x4_identity(m);
  mat4x4_ortho(p, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
  mat4x4_mul(mvp, p, m);

  vec2 resolution = {(float)context.width, (float)context.height};

  // update uniforms
  glUseProgram(program.program);
  glUniformMatrix4fv(program.mvp_location, 1, GL_FALSE, (const GLfloat *)&mvp);
  glUniform1f(program.itime_location, (const GLfloat)context.time);
  glUniform2fv(program.ires_location, 1, (const GLfloat *)&resolution);

  // start vertex handling
  glBindVertexArray(program.vertex_array);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}