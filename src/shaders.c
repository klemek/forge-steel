#include <glad/gl.h>
#include <linmath.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "config_file.h"
#include "constants.h"
#include "logs.h"
#include "shaders.h"
#include "types.h"

static bool compile_shader(GLuint shader_id, char *name, char *source_code) {
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

static void init_textures(ShaderProgram *program, Context context) {
  unsigned int i;

  glGenTextures(TEX_COUNT, program->textures);

  for (i = 0; i < TEX_COUNT; i++) {
    // selects which texture unit subsequent texture state calls will affect
    glActiveTexture(GL_TEXTURE0 + i);

    glBindTexture(GL_TEXTURE_2D, program->textures[i]);

    // define texture image as empty
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, context.width, context.height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, 0);

    // setup mipmap context
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    log_success("Texture %d initialized", i);
  }
}

static void init_framebuffers(ShaderProgram *program) {
  unsigned int i, j;

  program->frame_buffers = malloc(program->frag_count * sizeof(GLuint));

  glGenFramebuffers(program->frag_count, program->frame_buffers);

  for (i = 0; i < program->frag_count; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, program->frame_buffers[i]);

    for (j = 0; j < TEX_COUNT; j++) {
      // attaches a selected mipmap level or image of a texture object as one of
      // the logical buffers of the framebuffer object
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + j,
                             GL_TEXTURE_2D, program->textures[j], 0);
    }

    // check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      log_error("Framebuffer %d is KO: %x", i + 1,
                glCheckFramebufferStatus(GL_FRAMEBUFFER));

      program->error = true;

      return;
    }

    log_success("Framebuffer %d initialized", i);
  }

  return;
}

static void init_vertices(ShaderProgram *program) {
  // create vertex buffer and setup vertices
  glGenBuffers(1, &program->vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, program->vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // create vertex array
  glGenVertexArrays(1, &program->vertex_array);
  glBindVertexArray(program->vertex_array);
}

static void init_shaders(ShaderProgram *program, File *fragment_shaders) {
  unsigned int i;

  // compile vertex shader
  program->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  program->error |= !compile_shader(
      program->vertex_shader, "internal vertex shader", vertex_shader_text);

  // compile output fragment shader
  program->output_shader = glCreateShader(GL_FRAGMENT_SHADER);
  program->error |=
      !compile_shader(program->output_shader,
                      "internal fragment shader (output)", output_shader_text);

  // compile output fragment shader
  program->monitor_shader = glCreateShader(GL_FRAGMENT_SHADER);
  program->error |= !compile_shader(program->monitor_shader,
                                    "internal fragment shader (monitor)",
                                    monitor_shader_text);

  program->fragment_shaders = malloc(program->frag_count * sizeof(GLuint));

  // compile fragment shaders
  for (i = 0; i < program->frag_count; i++) {
    program->fragment_shaders[i] = glCreateShader(GL_FRAGMENT_SHADER);
    program->error |=
        !compile_shader(program->fragment_shaders[i], fragment_shaders[i].path,
                        fragment_shaders[i].content);

    if (program->error) {
      return;
    }
  }
}

static void init_single_program(ShaderProgram *program, unsigned int i,
                                ConfigFile shader_config, bool output) {
  unsigned int j;
  char name[32];

  program->programs[i] = glCreateProgram();

  glAttachShader(program->programs[i], program->vertex_shader);

  if (output) {
    glAttachShader(program->programs[i], program->monitor_shader); // TODO tmp
  } else {
    glAttachShader(program->programs[i], program->fragment_shaders[i]);
  }

  glLinkProgram(program->programs[i]);

  // create uniforms pointers
  if (!output) {
    program->itime_locations[i] = glGetUniformLocation(
        program->programs[i],
        config_file_get_str(shader_config, "UNIFORM_TIME", "iTime"));
    program->itempo_locations[i] = glGetUniformLocation(
        program->programs[i],
        config_file_get_str(shader_config, "UNIFORM_TEMPO", "iTempo"));
    program->ifps_locations[i] = glGetUniformLocation(
        program->programs[i],
        config_file_get_str(shader_config, "UNIFORM_FPS", "iFPS"));
    program->ires_locations[i] = glGetUniformLocation(
        program->programs[i],
        config_file_get_str(shader_config, "UNIFORM_RESOLUTION",
                            "iResolution"));

    for (j = 0; j < SUB_COUNT; j++) {
      sprintf(name, "src_%d", j + 1);
      program->sub_src_indexes[i][j] =
          glGetSubroutineIndex(program->programs[i], GL_FRAGMENT_SHADER, name);
      sprintf(name, "fx_%d", j + 1);
      program->sub_fx_indexes[i][j] =
          glGetSubroutineIndex(program->programs[i], GL_FRAGMENT_SHADER, name);
      if (j < 2) {
        sprintf(name, "mix_%d", j + 1);
        program->sub_mix_indexes[i][j] = glGetSubroutineIndex(
            program->programs[i], GL_FRAGMENT_SHADER, name);
      }
    }
  }

  // create texX uniforms pointer
  for (j = 0; j < TEX_COUNT; j++) {
    sprintf(name, "tex%d", j);
    program->textures_locations[j][i] =
        glGetUniformLocation(program->programs[i], name);
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

  log_success("Program %d initialized", i + 1);
}

static void init_programs(ShaderProgram *program, ConfigFile shader_config) {
  unsigned int i;

  program->programs = malloc(program->frag_count * sizeof(GLuint));
  program->itime_locations = malloc(program->frag_count * sizeof(GLuint));
  program->itempo_locations = malloc(program->frag_count * sizeof(GLuint));
  program->ifps_locations = malloc(program->frag_count * sizeof(GLuint));
  program->ires_locations = malloc(program->frag_count * sizeof(GLuint));
  program->vpos_locations = malloc(program->frag_count * sizeof(GLuint));

  for (i = 0; i < TEX_COUNT; i++) {
    program->textures_locations[i] =
        malloc(program->frag_count * sizeof(GLuint));
  }

  for (i = 0; i < program->frag_count + 1; i++) {
    init_single_program(program, i, shader_config, i == program->frag_count);
  }
}

static void init_drawbuffers(ShaderProgram *program) {
  unsigned int i;

  for (i = 0; i < TEX_COUNT; i++) {
    program->draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
  }
}

ShaderProgram shaders_init(File *fragment_shaders, ConfigFile shader_config,
                           Context context) {
  ShaderProgram program;

  program.error = false;
  program.last_width = context.width;
  program.last_height = context.height;
  program.frag_count = config_file_get_int(shader_config, "FRAG_COUNT", 6);

  init_textures(&program, context);

  init_framebuffers(&program);

  init_shaders(&program, fragment_shaders);

  if (program.error) {
    return program;
  }

  init_vertices(&program);

  init_programs(&program, shader_config);

  init_drawbuffers(&program);

  return program;
}

void shaders_update(ShaderProgram program, File *fragment_shaders,
                    unsigned int i) {
  bool result;

  result = compile_shader(program.fragment_shaders[i], fragment_shaders[i].path,
                          fragment_shaders[i].content);

  if (result) {
    glLinkProgram(program.programs[i]);

    log_success("Program %d updated", i + 1);
  }
}

void shaders_apply(ShaderProgram program, Context context) {
  unsigned int i, j;
  GLuint subroutines[3];
  vec2 resolution;

  // viewport changed
  if (context.width != program.last_width ||
      context.height != program.last_height) {
    // update viewport
    glViewport(0, 0, context.width, context.height);

    // clean and resize all textures
    for (i = 0; i < TEX_COUNT; i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, context.width, context.height, 0,
                   GL_RGB, GL_UNSIGNED_BYTE, 0);
    }
  }

  resolution[0] = (float)context.width;
  resolution[1] = (float)context.height;

  for (i = 0; i < program.frag_count + 1; i++) {
    // use specific shader program
    glUseProgram(program.programs[i]);

    if (i == program.frag_count) {
      // use default framebuffer (output)
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      // clear buffer
      glClear(GL_COLOR_BUFFER_BIT);
    } else {
      // use memory framebuffer
      glBindFramebuffer(GL_FRAMEBUFFER, program.frame_buffers[0]);

      // set fragment uniforms
      glUniform1f(program.itime_locations[i], (const GLfloat)context.time);
      glUniform1f(program.itempo_locations[i],
                  (const GLfloat)120.0f); // TODO TMP
      glUniform1i(program.ifps_locations[i], (const GLint)context.fps);
      glUniform2fv(program.ires_locations[i], 1, (const GLfloat *)&resolution);

      // TODO tmp
      subroutines[0] = program.sub_src_indexes[i][i == 0 ? 1 : 2];
      subroutines[1] = program.sub_fx_indexes[i][0];
      subroutines[2] = program.sub_mix_indexes[i][0];

      glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 3, subroutines);
    }

    // set GL_TEXTURE(X) to uniform sampler2D texX
    for (j = 0; j < TEX_COUNT; j++) {
      glUniform1i(program.textures_locations[j][i], j);
    }

    glDrawBuffers(TEX_COUNT, program.draw_buffers);

    // draw output
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}
