#include <glad/gl.h>
#include <linmath.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

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

  program->textures = malloc(program->tex_count * sizeof(GLuint));

  glGenTextures(program->tex_count, program->textures);

  for (i = 0; i < program->tex_count; i++) {
    // selects which texture unit subsequent texture state calls will affect
    glActiveTexture(GL_TEXTURE0 + i);

    glBindTexture(GL_TEXTURE_2D, program->textures[i]);

    // define texture image as empty
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB,
        (int)(context.internal_size * (float)context.width / context.height),
        context.internal_size, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    // setup mipmap context
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    log_success("Texture %d initialized", i);
  }
}

static void init_framebuffers(ShaderProgram *program,
                              ConfigFile shader_config) {
  unsigned int i;
  unsigned tex_i;
  char name[32];

  program->frame_buffers = malloc(program->frag_count * sizeof(GLuint));

  glGenFramebuffers(program->frag_count, program->frame_buffers);

  for (i = 0; i < program->frag_count; i++) {
    if (i == program->frag_output_index || i == program->frag_monitor_index) {
      continue;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, program->frame_buffers[i]);

    sprintf(name, "FRAG_%d_OUT", i + 1);
    tex_i = config_file_get_int(shader_config, name, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           program->textures[tex_i], 0);

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
                                ConfigFile shader_config) {
  unsigned int j, k;
  char name[32];
  char *tex_prefix;
  char *sub_prefix;
  char *seed_prefix;
  char *state_prefix;

  program->programs[i] = glCreateProgram();

  glAttachShader(program->programs[i], program->vertex_shader);
  glAttachShader(program->programs[i], program->fragment_shaders[i]);
  glLinkProgram(program->programs[i]);

  // create uniforms pointers
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
      config_file_get_str(shader_config, "UNIFORM_RESOLUTION", "iResolution"));
  program->idemo_locations[i] = glGetUniformLocation(
      program->programs[i],
      config_file_get_str(shader_config, "UNIFORM_DEMO", "iDemo"));

  seed_prefix =
      config_file_get_str(shader_config, "UNIFORM_SEED_PREFIX", "seed");
  for (j = 0; j < program->frag_count; j++) {
    sprintf(name, "%s%d", seed_prefix, j + 1);
    program->iseed_locations[i * program->frag_count + j] =
        glGetUniformLocation(program->programs[i], name);
  }

  state_prefix =
      config_file_get_str(shader_config, "UNIFORM_STATE_PREFIX", "state");
  for (j = 0; j < program->frag_count; j++) {
    for (k = 0; k < program->sub_type_count; k++) {
      sprintf(name, "%s%d_%d", state_prefix, j + 1, k + 1);
      program
          ->istate_locations[i * program->frag_count * program->sub_type_count +
                             j * program->sub_type_count + k] =
          glGetUniformLocation(program->programs[i], name);
    }
  }

  for (j = 0; j < program->sub_type_count; j++) {
    sprintf(name, "SUB_%d_PREFIX", j + 1);
    sub_prefix = config_file_get_str(shader_config, name, 0);
    for (k = 0; k < program->sub_variant_count; k++) {
      sprintf(name, "%s%d", sub_prefix, k + 1);
      program->sub_locations[i * program->sub_variant_count *
                                 program->sub_type_count +
                             j * program->sub_variant_count + k] =
          glGetSubroutineIndex(program->programs[i], GL_FRAGMENT_SHADER, name);
    }
  }

  // create texX uniforms pointer
  tex_prefix = config_file_get_str(shader_config, "UNIFORM_TEX_PREFIX", "tex");
  for (j = 0; j < program->tex_count; j++) {
    sprintf(name, "%s%d", tex_prefix, j);
    program->textures_locations[i * program->tex_count + j] =
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
  program->idemo_locations = malloc(program->frag_count * sizeof(GLuint));
  program->iseed_locations =
      malloc(program->frag_count * program->frag_count * sizeof(GLuint));
  program->istate_locations = malloc(program->frag_count * program->frag_count *
                                     program->sub_type_count * sizeof(GLuint));
  program->vpos_locations = malloc(program->frag_count * sizeof(GLuint));
  program->textures_locations =
      malloc(program->frag_count * program->tex_count * sizeof(GLuint));
  program->sub_locations =
      malloc(program->frag_count * program->sub_type_count *
             program->sub_variant_count * sizeof(GLuint));

  for (i = 0; i < program->frag_count; i++) {
    init_single_program(program, i, shader_config);
  }
}

ShaderProgram shaders_init(File *fragment_shaders, ConfigFile shader_config,
                           Context context) {
  ShaderProgram program;

  program.error = false;
  program.last_width = context.width;
  program.last_height = context.height;
  program.tex_count = config_file_get_int(shader_config, "TEX_COUNT", 9);
  program.frag_count = config_file_get_int(shader_config, "FRAG_COUNT", 6);
  program.frag_output_index =
      config_file_get_int(shader_config, "FRAG_OUTPUT", 1) - 1;
  program.frag_monitor_index =
      config_file_get_int(shader_config, "FRAG_MONITOR", 1) - 1;
  program.sub_type_count =
      config_file_get_int(shader_config, "SUB_TYPE_COUNT", 0);
  program.sub_variant_count =
      config_file_get_int(shader_config, "SUB_VARIANT_COUNT", 1);

  init_textures(&program, context);

  init_framebuffers(&program, shader_config);

  init_shaders(&program, fragment_shaders);

  if (program.error) {
    return program;
  }

  init_vertices(&program);

  init_programs(&program, shader_config);

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

static void update_viewport(ShaderProgram program, Context context) {
  unsigned int i;

  // viewport changed
  if (context.width != program.last_width ||
      context.height != program.last_height) {
    // clean and resize all textures
    for (i = 0; i < program.tex_count; i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      glTexImage2D(
          GL_TEXTURE_2D, 0, GL_RGB,
          (int)(context.internal_size * (float)context.width / context.height),
          context.internal_size, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    }
  }
}

static void use_program(ShaderProgram program, int i, bool output,
                        Context context) {
  unsigned int j, k;
  GLuint *subroutines;
  vec2 resolution;

  resolution[0] = (float)context.width;
  resolution[1] = (float)context.height;
  subroutines = malloc(program.sub_type_count * sizeof(GLuint));

  // use specific shader program
  glUseProgram(program.programs[i]);

  if (output) {
    glViewport(0, 0, context.width, context.height);

    // use default framebuffer (output)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT);
  } else {
    glViewport(
        0, 0,
        (int)(context.internal_size * (float)context.width / context.height),
        context.internal_size);

    // use memory framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, program.frame_buffers[i]);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
  }
  // set fragment uniforms
  glUniform1f(program.itime_locations[i], (const GLfloat)context.time);
  glUniform1f(program.itempo_locations[i], (const GLfloat)context.tempo);
  glUniform1i(program.ifps_locations[i], (const GLint)context.fps);
  glUniform1i(program.idemo_locations[i], (const GLint)(context.demo ? 1 : 0));
  glUniform2fv(program.ires_locations[i], 1, (const GLfloat *)&resolution);

  // set seeds uniforms
  for (j = 0; j < program.frag_count; j++) {
    glUniform1i(program.iseed_locations[i * program.frag_count + j],
                (const GLint)context.seeds[j]);
  }

  // set subroutines for fragment and update state uniforms
  for (j = 0; j < program.sub_type_count; j++) {
    k = context.sub_state[i * program.sub_type_count + j];
    subroutines[j] = program.sub_locations[i * program.sub_type_count *
                                               program.sub_variant_count +
                                           j * program.sub_variant_count + k];
    for (k = 0; k < program.frag_count; k++) {
      glUniform1i(
          program.istate_locations[i * program.frag_count *
                                       program.sub_type_count +
                                   k * program.sub_type_count + j],
          (const GLint)context.sub_state[k * program.sub_type_count + j]);
    }
  }

  glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, program.sub_type_count,
                          subroutines);

  // set GL_TEXTURE(X) to uniform sampler2D texX
  for (j = 0; j < program.tex_count; j++) {
    glUniform1i(program.textures_locations[i * program.tex_count + j], j);
  }

  // draw output
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void shaders_apply(ShaderProgram program, Context context) {
  unsigned int i;

  update_viewport(program, context);

  for (i = 0; i < program.frag_count; i++) {
    if (i != program.frag_output_index && i != program.frag_monitor_index) {
      use_program(program, i, false, context);
    }
  }

  use_program(program,
              context.monitor ? program.frag_monitor_index
                              : program.frag_output_index,
              true, context);
}
