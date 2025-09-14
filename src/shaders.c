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

bool init_textures(ShaderProgram *program, Context context) {
  int i;

  glGenTextures(BUFFER_COUNT, program->textures);

  for (i = 0; i < BUFFER_COUNT; i++) {
    // selects which texture unit subsequent texture state calls will affect
    glActiveTexture(GL_TEXTURE0 + i);

    glBindTexture(GL_TEXTURE_2D, program->textures[i]);

    // define texture image as empty
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, context.width, context.height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, 0);

    // setup mipmap context
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  }

  return true;
}

void init_framebuffers(ShaderProgram *program) {
  int i;

  glGenFramebuffers(BUFFER_COUNT, program->frame_buffers);

  for (i = 0; i < BUFFER_COUNT; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, program->frame_buffers[i]);

    // attaches a selected mipmap level or image of a texture object as one of
    // the logical buffers of the framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           program->textures[i], 0);

    // check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      log_error("Framebuffer %d is KO: %x", i + 1,
                glCheckFramebufferStatus(GL_FRAMEBUFFER));

      program->error = true;

      return;
    }
  }

  return;
}

void init_vertices(ShaderProgram *program) {
  // create vertex buffer and setup vertices
  glGenBuffers(1, &program->vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, program->vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // create vertex array
  glGenVertexArrays(1, &program->vertex_array);
  glBindVertexArray(program->vertex_array);
}

void init_shaders(ShaderProgram *program, File fragment_shader) {
  // compile vertex shader
  program->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  program->error |= !compile_shader(
      program->vertex_shader, "internal vertex shader", vertex_shader_text);

  // compile output fragment shader
  program->output_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  program->error |=
      !compile_shader(program->output_fragment_shader,
                      "internal fragment shader", output_fragment_shader_text);

  // compile fragment shader
  program->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  program->error |= !compile_shader(
      program->fragment_shader, fragment_shader.path, fragment_shader.content);
}

void init_single_program(ShaderProgram *program, int i, bool output) {
  int j;
  char uniform_name[32];

  program->programs[i] = glCreateProgram();

  glAttachShader(program->programs[i], program->vertex_shader);

  if (output) {
    glAttachShader(program->programs[i], program->output_fragment_shader);
  } else {
    // TODO add others
    glAttachShader(program->programs[i], program->fragment_shader);
  }

  glLinkProgram(program->programs[i]);

  // create uniforms pointers
  if (!output) {
    program->itime_locations[i] =
        glGetUniformLocation(program->programs[i], "iTime");
    program->ires_locations[i] =
        glGetUniformLocation(program->programs[i], "iResolution");
  }

  // create frameX uniforms pointer
  for (j = 0; j < BUFFER_COUNT; j++) {
    sprintf(uniform_name, "frame%d", j);
    program->frames_locations[i][j] =
        glGetUniformLocation(program->programs[i], uniform_name);
  }

  // create attribute pointer
  program->vpos_locations[i] =
      glGetAttribLocation(program->programs[i], "vPos");
  // enable attribute pointer
  glEnableVertexAttribArray(program->vpos_locations[i]);
  // specify the location and data format of the array of generic vertex
  // attributes to use when rendering
  glVertexAttribPointer(program->vpos_locations[i], 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void *)offsetof(Vertex, pos));

  log_success("Program %d initialized", i);
}

ShaderProgram init_program(File fragment_shader, Context context) {
  int i, j;
  char uniform_name[32];

  ShaderProgram program = {.error = false,
                           .last_width = context.width,
                           .last_height = context.height};

  init_textures(&program, context);

  init_framebuffers(&program);

  init_shaders(&program, fragment_shader);

  if (program.error) {
    return program;
  }

  init_vertices(&program);

  // create and link full shader programs
  for (i = 0; i < BUFFER_COUNT + 1; i++) {
    init_single_program(&program, i, i == BUFFER_COUNT);
  }

  return program;
}

void update_program(ShaderProgram program, File fragment_shader) {
  bool result;
  int i;

  result = compile_shader(program.fragment_shader, fragment_shader.path,
                          fragment_shader.content);

  if (result) {
    // re-link all programs
    for (i = 0; i < BUFFER_COUNT; i++) {
      glLinkProgram(program.programs[i]);
    }

    log_success("Programs updated");
  }
}

void apply_program(ShaderProgram program, Context context) {
  int i, j;

  // viewport changed
  if (context.width != program.last_width ||
      context.height != program.last_height) {
    // update viewport
    glViewport(0, 0, context.width, context.height);

    // clean and resize all textures
    for (i = 0; i < BUFFER_COUNT; i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, context.width, context.height, 0,
                   GL_RGB, GL_UNSIGNED_BYTE, 0);
    }
  }

  vec2 resolution = {(float)context.width, (float)context.height};

  for (i = 0; i < BUFFER_COUNT + 1; i++) {
    // use specific shader program
    glUseProgram(program.programs[i]);

    if (i == BUFFER_COUNT) {
      // use default framebuffer (output)
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      // clear buffer
      glClear(GL_COLOR_BUFFER_BIT);
    } else {
      // use memory framebuffer
      glBindFramebuffer(GL_FRAMEBUFFER, program.frame_buffers[0]);

      // set fragment uniforms
      glUniform1f(program.itime_locations[i], (const GLfloat)context.time);
      glUniform2fv(program.ires_locations[i], 1, (const GLfloat *)&resolution);
    }

    // set GL_TEXTURE(X) to uniform sampler2D frameX
    for (j = 0; j < BUFFER_COUNT; j++) {
      glUniform1i(program.frames_locations[i][j], j);
    }

    // draw output
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}

// TODO clean buffers from opengl memories