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
  char uniform_name[32];

  ShaderProgram program = {.error = false,
                           .last_width = context.width,
                           .last_height = context.height};

  // create empty textures
  glGenTextures(BUFFER_COUNT, program.textures);

  for (i = 0; i < BUFFER_COUNT; i++) {
    glActiveTexture(GL_TEXTURE0 + i);

    glBindTexture(GL_TEXTURE_2D, program.textures[i]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, context.width, context.height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  }

  // create frame buffers
  glGenFramebuffers(BUFFER_COUNT, program.frame_buffers);

  for (i = 0; i < BUFFER_COUNT; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, program.frame_buffers[i]);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           program.textures[i], 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      log_error("Framebuffer %d is KO: %x", i + 1,
                glCheckFramebufferStatus(GL_FRAMEBUFFER));
      program.error = true;
      return program;
    }
  }

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

  // create vertex buffer and setup vertices
  glGenBuffers(1, &program.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, program.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // create vertex array
  glGenVertexArrays(1, &program.vertex_array);
  glBindVertexArray(program.vertex_array);

  // create and link full shader program
  for (i = 0; i < BUFFER_COUNT + 1; i++) {
    program.programs[i] = glCreateProgram();
    glAttachShader(program.programs[i], program.vertex_shader);
    if (i == BUFFER_COUNT) {
      glAttachShader(program.programs[i], program.output_fragment_shader);
    } else {
      // TODO add others
      glAttachShader(program.programs[i], program.fragment_shader);
    }
    glLinkProgram(program.programs[i]);

    // create uniforms pointers
    if (i != BUFFER_COUNT) {
      program.itime_locations[i] =
          glGetUniformLocation(program.programs[i], "iTime");
      program.ires_locations[i] =
          glGetUniformLocation(program.programs[i], "iResolution");
    }

    for (j = 0; j < BUFFER_COUNT; j++) {
      sprintf(uniform_name, "frame%d", j);
      program.frames_locations[i][j] =
          glGetUniformLocation(program.programs[i], uniform_name);
    }

    // create attribute pointer
    program.vpos_locations[i] =
        glGetAttribLocation(program.programs[i], "vPos");

    glEnableVertexAttribArray(program.vpos_locations[i]);
    glVertexAttribPointer(program.vpos_locations[i], 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void *)offsetof(Vertex, pos));

    log_success("Program %d initialized", i);
  }

  return program;
}

void update_program(ShaderProgram program, File fragment_shader) {
  bool result;
  int i;

  result = compile_shader(program.fragment_shader, fragment_shader.path,
                          fragment_shader.content);

  if (result) {
    // re-link program
    for (i = 0; i < BUFFER_COUNT; i++) {
      glLinkProgram(program.programs[i]);
    }

    log_success("Programs updated");
  }
}

void apply_program(ShaderProgram program, Context context) {
  int i, j;

  if (context.width != program.last_width ||
      context.height != program.last_height) {
    glViewport(0, 0, context.width, context.height);

    for (i = 0; i < BUFFER_COUNT; i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, context.width, context.height, 0,
                   GL_RGB, GL_UNSIGNED_BYTE, 0);
    }
  }

  vec2 resolution = {(float)context.width, (float)context.height};

  for (i = 0; i < BUFFER_COUNT + 1; i++) {
    glUseProgram(program.programs[i]);

    if (i == BUFFER_COUNT) {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glClear(GL_COLOR_BUFFER_BIT);
    } else {
      glBindFramebuffer(GL_FRAMEBUFFER, program.frame_buffers[0]);

      glUniform1f(program.itime_locations[i], (const GLfloat)context.time);
      glUniform2fv(program.ires_locations[i], 1, (const GLfloat *)&resolution);
    }

    for (j = 0; j < BUFFER_COUNT; j++) {
      glUniform1i(program.frames_locations[i][j], j);
    }

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}

// TODO clean buffers from opengl memories