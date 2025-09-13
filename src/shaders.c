#include <glad/gl.h>
#include <linmath.h>
#include <stddef.h>
#include <stdio.h>

#include "constants.h"
#include "types.h"

// TODO split into smaller functions
ShaderProgram init_program(File fragment_shader) {
  ShaderProgram program = {};
  GLint status_params;

  glGenBuffers(1, &program.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, program.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  program.vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(program.vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShader(program.vertex_shader);

  glGetShaderiv(program.vertex_shader, GL_COMPILE_STATUS, &status_params);
  if (status_params == GL_FALSE) {
    program.error = true;
    // TODO use glGetShaderInfoLog( 	GLuint shader, GLsizei
    // maxLength, GLsizei *length, GLchar *infoLog);
  }

  program.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(program.fragment_shader, 1,
                 (const GLchar *const *)&fragment_shader.content, NULL);
  glCompileShader(program.fragment_shader);

  glGetShaderiv(program.fragment_shader, GL_COMPILE_STATUS, &status_params);
  if (status_params == GL_FALSE) {
    program.error = true;
  }

  if (program.error) {
    return program;
  }

  program.program = glCreateProgram();
  glAttachShader(program.program, program.vertex_shader);
  glAttachShader(program.program, program.fragment_shader);
  glLinkProgram(program.program);

  program.mvp_location = glGetUniformLocation(program.program, "mvp");
  program.itime_location = glGetUniformLocation(program.program, "iTime");
  program.ires_location = glGetUniformLocation(program.program, "iResolution");
  program.vpos_location = glGetAttribLocation(program.program, "vPos");

  glGenVertexArrays(1, &program.vertex_array);
  glBindVertexArray(program.vertex_array);
  glEnableVertexAttribArray(program.vpos_location);
  glVertexAttribPointer(program.vpos_location, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void *)offsetof(Vertex, pos));

  return program;
}

void update_program(ShaderProgram program, File fragment_shader) {
  GLint status_params;
  glShaderSource(program.fragment_shader, 1,
                 (const GLchar *const *)&fragment_shader.content, NULL);
  glCompileShader(program.fragment_shader);

  glGetShaderiv(program.fragment_shader, GL_COMPILE_STATUS, &status_params);
  if (status_params == GL_FALSE) {
    fprintf(stderr, "Failed to compile shaders\n"); // TODO add info
    return;
  }
  glLinkProgram(program.program);
}