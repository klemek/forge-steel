#include <glad/gl.h>
#include <linmath.h>
#include <stddef.h>

#include "config.h"
#include "constants.h"
#include "logs.h"
#include "types.h"

bool compile_shader(GLuint shader_id, char *name, char *source_code) {
  GLint status_params;
  char log[1024];

  log_info("Compiling '%s'...", name);

  // update shader source code
  glShaderSource(shader_id, 1, (const GLchar **)&source_code, NULL);

  // compile shader
  glCompileShader(shader_id);

  // get compilation status
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status_params);
  glGetShaderInfoLog(shader_id, 1024, NULL, (GLchar *)&log);

  if (status_params == GL_FALSE) {
    log_error("Failed to compile\n%s", log);
  } else {
    log_success("Compilation successful");
  }

  return status_params == GL_TRUE;
}

ShaderProgram init_program(File fragment_shader, Context context) {
  int i, j;
  GLenum draw_buffers[BUFFER_COUNT];
  char uniform_name[32];

  for (i = 0; i < BUFFER_COUNT; i++) {
    draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
  }

  ShaderProgram program = {.error = false, .frame_buffers = FRAMEBUFFER_IDS};

  // create vertex buffer and setup vertices
  glGenBuffers(1, &program.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, program.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // create framebuffers and textures
  glGenTextures(1, program.textures);
  glGenFramebuffers(BUFFER_COUNT, program.frame_buffers);
  glGenRenderbuffers(BUFFER_COUNT, program.render_buffers);
  for (i = 0; i < BUFFER_COUNT; i++) {
    glTextureParameteri(program.textures[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(program.textures[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(program.textures[i], GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
    glTextureParameteri(program.textures[i], GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, program.textures[i]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, context.width, context.height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, program.render_buffers[i]);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, context.width,
                          context.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, program.render_buffers[i]);

    glBindFramebuffer(GL_FRAMEBUFFER, program.frame_buffers[i]);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, program.render_buffers[i]);

    for (j = 0; j < BUFFER_COUNT; j++) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + j,
                             GL_TEXTURE_2D, program.textures[j], 0);
    }

    glDrawBuffers(BUFFER_COUNT, draw_buffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      log_error("Framebuffer %d is KO: %x", i,
                glCheckFramebufferStatus(GL_FRAMEBUFFER));
      program.error = true;
      return program;
    }
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // compile vertex shader
  program.vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  program.error |= !compile_shader(
      program.vertex_shader, "internal vertex shader", vertex_shader_text);

  // compile fragment shader
  program.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  program.error |= !compile_shader(
      program.fragment_shader, fragment_shader.path, fragment_shader.content);

  program.output_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  program.error |=
      !compile_shader(program.output_fragment_shader,
                      "internal fragment shader", output_fragment_shader_text);

  if (program.error) {
    return program;
  }

  // create and link full shader program
  program.program = glCreateProgram();
  glAttachShader(program.program, program.vertex_shader);
  glAttachShader(program.program, program.fragment_shader);
  glAttachShader(program.program, program.output_fragment_shader);
  glLinkProgram(program.program);

  // create uniforms pointers
  program.mvp_location = glGetUniformLocation(program.program, "mvp");
  program.itime_location = glGetUniformLocation(program.program, "iTime");
  program.ires_location = glGetUniformLocation(program.program, "iResolution");

  for (i = 0; i < BUFFER_COUNT; i++) {
    sprintf(uniform_name, "frame%d", i);
    program.frames_location[i] =
        glGetUniformLocation(program.program, uniform_name);
  }

  // create attribute pointer
  program.vpos_location = glGetAttribLocation(program.program, "vPos");

  // create vertex array and bind to vPos attribute
  glGenVertexArrays(1, &program.vertex_array);
  glBindVertexArray(program.vertex_array);
  glEnableVertexAttribArray(program.vpos_location);
  glVertexAttribPointer(program.vpos_location, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void *)offsetof(Vertex, pos));

  log_success("Program initialized");

  return program;
}

void update_program(ShaderProgram program, File fragment_shader) {
  bool result;

  result = compile_shader(program.fragment_shader, fragment_shader.path,
                          fragment_shader.content);

  if (result) {
    // re-link program
    glLinkProgram(program.program);

    log_success("Program updated");
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