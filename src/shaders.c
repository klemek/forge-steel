#include <linmath.h>
#include <log.h>
#include <stddef.h>
#include <stdio.h>

#include "config.h"
#include "config_file.h"
#include "constants.h"
#include "shaders.h"
#include "types.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLAD_EGL_IMPLEMENTATION
#include <glad/egl.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

static const GLuint unused_uniform = (GLuint)-1;

static void init_gl(ShaderProgram *program) {
  gladLoadGL(glfwGetProcAddress);

  program->egl_display = glfwGetEGLDisplay();
  if (program->egl_display == EGL_NO_DISPLAY) {
    log_error("error: glfwGetEGLDisplay no EGLDisplay returned");
    program->error = true;
    return;
  }

  gladLoadEGL(program->egl_display, glfwGetProcAddress);
}

static void init_textures(ShaderProgram *program, SharedContext *context) {
  unsigned int i;

  glGenTextures(program->tex_count, program->textures);

  for (i = 0; i < program->tex_count; i++) {
    // selects which texture unit subsequent texture state calls will affect
    glActiveTexture(GL_TEXTURE0 + i);

    glBindTexture(GL_TEXTURE_2D, program->textures[i]);

    glEnable(GL_TEXTURE_2D);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    // define texture image as empty
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, context->internal_width,
                 context->internal_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    // setup mipmap context
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    log_info("Texture %d initialized", i);
  }
}

static void rebind_textures(ShaderProgram *program) {
  unsigned int i;
  for (i = 0; i < program->tex_count; i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, program->textures[i]);
  }
}

static void link_input_to_texture(ShaderProgram *program, VideoCapture *input,
                                  unsigned int texture_index) {
  input->dma_image = EGL_NO_IMAGE_KHR;

  // https://registry.khronos.org/EGL/extensions/EXT/EGL_EXT_image_dma_buf_import.txt
  const EGLint attrib_list[] = {EGL_WIDTH,
                                input->width,
                                EGL_HEIGHT,
                                input->height,
                                EGL_LINUX_DRM_FOURCC_EXT,
                                input->pixelformat,
                                EGL_DMA_BUF_PLANE0_FD_EXT,
                                input->exp_fd,
                                EGL_DMA_BUF_PLANE0_OFFSET_EXT,
                                0,
                                EGL_DMA_BUF_PLANE0_PITCH_EXT,
                                input->bytesperline,
                                EGL_NONE};

  input->dma_image = eglCreateImageKHR(program->egl_display, EGL_NO_CONTEXT,
                                       EGL_LINUX_DMA_BUF_EXT,
                                       (EGLClientBuffer)NULL, attrib_list);

  if (input->dma_image == EGL_NO_IMAGE_KHR) {
    log_error("(%s) eglCreateImageKHR failed %04x", input->name, eglGetError());
    return;
  }

  glActiveTexture(GL_TEXTURE0 + texture_index);

  glBindTexture(GL_TEXTURE_2D, program->textures[texture_index]);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, input->width, input->height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, 0);

  // https://registry.khronos.org/OpenGL/extensions/EXT/EXT_EGL_image_storage.txt
  glEGLImageTargetTexStorageEXT(GL_TEXTURE_2D, (GLeglImageOES)input->dma_image,
                                NULL);

  log_info("Texture %d linked to %s", texture_index, input->name);
}

static void init_input(ShaderProgram *program, ConfigFile config,
                       VideoCapture *inputs, unsigned int input_count) {
  unsigned int i;
  unsigned tex_i;
  char name[256];

  for (i = 0; i < program->in_count; i++) {
    if (i < input_count && !inputs[i].error) {
      sprintf(name, "IN_%d_OUT", i + 1);
      tex_i = config_file_get_int(config, name, 0);
      link_input_to_texture(program, &inputs[i], tex_i);
    } else {
      log_warn("Cannot link input %d", i + 1);
    }
  }
}

static void init_framebuffers(ShaderProgram *program, ConfigFile config) {
  unsigned int i;
  unsigned tex_i;
  char name[256];

  glGenFramebuffers(program->frag_count, program->frame_buffers);

  for (i = 0; i < program->frag_count; i++) {
    if (i == program->frag_output_index || i == program->frag_monitor_index) {
      continue;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, program->frame_buffers[i]);

    sprintf(name, "FRAG_%d_OUT", i + 1);
    tex_i = config_file_get_int(config, name, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           program->textures[tex_i], 0);

    // check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      log_error("Framebuffer %d is KO: %x", i + 1,
                glCheckFramebufferStatus(GL_FRAMEBUFFER));

      program->error = true;

      return;
    }

    log_info("Framebuffer %d initialized", i);
  }

  return;
}

static void init_vertices(ShaderProgram *program) {
  glGenBuffers(1, &program->vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, program->vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

static void bind_vertices(ShaderProgram *program, unsigned int index) {
  unsigned int i;

  glBindBuffer(GL_ARRAY_BUFFER, program->vertex_buffer);

  glGenVertexArrays(1, &program->vertex_array[index]);
  glBindVertexArray(program->vertex_array[index]);

  for (i = 0; i < program->frag_count; i++) {
    // enable attribute pointer
    glEnableVertexAttribArray(program->vpos_locations[i]);
    // specify the location and data format of the array of generic vertex
    // attributes to use when rendering
    glVertexAttribPointer(program->vpos_locations[i], 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void *)offsetof(Vertex, pos));
  }
}

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
    log_info("Compilation successful");
  }

  return status_params == GL_TRUE;
}

static void init_shaders(ShaderProgram *program, File *fragment_shaders) {
  unsigned int i;

  // compile vertex shader
  program->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  program->error |= !compile_shader(
      program->vertex_shader, "internal vertex shader", vertex_shader_text);

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
                                ConfigFile config) {
  unsigned int j, k;
  char name[256];
  char *prefix;
  program->programs[i] = glCreateProgram();

  glAttachShader(program->programs[i], program->vertex_shader);
  glAttachShader(program->programs[i], program->fragment_shaders[i]);
  glLinkProgram(program->programs[i]);

  // create uniforms pointers
  program->itime_locations[i] = glGetUniformLocation(
      program->programs[i],
      config_file_get_str(config, "UNIFORM_TIME", "iTime"));
  program->itempo_locations[i] = glGetUniformLocation(
      program->programs[i],
      config_file_get_str(config, "UNIFORM_TEMPO", "iTempo"));
  program->ifps_locations[i] = glGetUniformLocation(
      program->programs[i], config_file_get_str(config, "UNIFORM_FPS", "iFPS"));
  program->ires_locations[i] = glGetUniformLocation(
      program->programs[i],
      config_file_get_str(config, "UNIFORM_RESOLUTION", "iResolution"));
  program->itexres_locations[i] = glGetUniformLocation(
      program->programs[i],
      config_file_get_str(config, "UNIFORM_TEX_RESOLUTION", "iTexResolution"));
  program->idemo_locations[i] = glGetUniformLocation(
      program->programs[i],
      config_file_get_str(config, "UNIFORM_DEMO", "iDemo"));
  program->ipage_locations[i] = glGetUniformLocation(
      program->programs[i],
      config_file_get_str(config, "UNIFORM_PAGE", "iPage"));
  program->iselected_locations[i] = glGetUniformLocation(
      program->programs[i],
      config_file_get_str(config, "UNIFORM_SELECTED", "iSelected"));

  prefix = config_file_get_str(config, "UNIFORM_IN_RESOLUTION_PREFIX",
                               "iInputResolution");
  for (j = 0; j < program->in_count; j++) {
    sprintf(name, "%s%d", prefix, j + 1);
    program->iinres_locations[i * program->in_count + j] =
        glGetUniformLocation(program->programs[i], name);
  }

  prefix =
      config_file_get_str(config, "UNIFORM_IN_FORMAT_PREFIX", "iInputFormat");
  for (j = 0; j < program->in_count; j++) {
    sprintf(name, "%s%d", prefix, j + 1);
    program->iinfmt_locations[i * program->in_count + j] =
        glGetUniformLocation(program->programs[i], name);
  }

  prefix = config_file_get_str(config, "UNIFORM_IN_FPS_PREFIX", "iInputFPS");
  for (j = 0; j < program->in_count; j++) {
    sprintf(name, "%s%d", prefix, j + 1);
    program->iinfps_locations[i * program->in_count + j] =
        glGetUniformLocation(program->programs[i], name);
  }

  prefix = config_file_get_str(config, "UNIFORM_SEED_PREFIX", "seed");
  for (j = 0; j < program->frag_count; j++) {
    sprintf(name, "%s%d", prefix, j + 1);
    program->iseed_locations[i * program->frag_count + j] =
        glGetUniformLocation(program->programs[i], name);
  }

  prefix = config_file_get_str(config, "UNIFORM_STATE_PREFIX", "state");
  for (j = 0; j < program->frag_count; j++) {
    sprintf(name, "%s%d", prefix, j + 1);
    program->istate_locations[i * program->frag_count + j] =
        glGetUniformLocation(program->programs[i], name);
  }

  for (j = 0; j < program->sub_type_count; j++) {
    sprintf(name, "SUB_%d_PREFIX", j + 1);
    prefix = config_file_get_str(config, name, 0);
    for (k = 0; k < program->sub_variant_count; k++) {
      sprintf(name, "%s%d", prefix, k + 1);
      program->sub_locations[i * program->sub_variant_count *
                                 program->sub_type_count +
                             j * program->sub_variant_count + k] =
          glGetSubroutineIndex(program->programs[i], GL_FRAGMENT_SHADER, name);
    }
  }

  prefix = config_file_get_str(config, "UNIFORM_ACTIVE_PREFIX", "active");
  for (j = 0; j < program->active_count; j++) {
    sprintf(name, "%s%d", prefix, j + 1);
    program->iactive_locations[i * program->active_count + j] =
        glGetUniformLocation(program->programs[i], name);
  }

  // create texX uniforms pointer
  prefix = config_file_get_str(config, "UNIFORM_TEX_PREFIX", "tex");
  for (j = 0; j < program->tex_count; j++) {
    sprintf(name, "%s%d", prefix, j);
    program->textures_locations[i * program->tex_count + j] =
        glGetUniformLocation(program->programs[i], name);
  }

  // create attribute pointer
  program->vpos_locations[i] =
      glGetAttribLocation(program->programs[i], "vPos");

  log_info("Program %d initialized", i + 1);
}

static void init_programs(ShaderProgram *program, ConfigFile config) {
  unsigned int i;

  for (i = 0; i < program->frag_count; i++) {
    init_single_program(program, i, config);
  }
}

ShaderProgram shaders_init(File *fragment_shaders, ConfigFile config,
                           SharedContext *context, VideoCapture *inputs,
                           unsigned int input_count,
                           unsigned int sub_variant_count,
                           unsigned int active_count, ShaderProgram *previous) {
  ShaderProgram program;

  if (previous == NULL) {
    program.error = false;
    program.last_width = context->width;
    program.last_height = context->height;
    program.tex_count = config_file_get_int(config, "TEX_COUNT", 9);
    program.frag_count = config_file_get_int(config, "FRAG_COUNT", 10);
    program.frag_output_index =
        config_file_get_int(config, "FRAG_OUTPUT", 1) - 1;
    program.frag_monitor_index =
        config_file_get_int(config, "FRAG_MONITOR", 1) - 1;
    program.sub_type_count = config_file_get_int(config, "SUB_TYPE_COUNT", 0);
    program.in_count = config_file_get_int(config, "IN_COUNT", 0);
    program.sub_variant_count = sub_variant_count;
    program.active_count = active_count;

    if (program.frag_count > MAX_FRAG) {
      log_error("FRAG_COUNT over %d", MAX_FRAG);
      program.error = true;
      return program;
    }

    init_gl(&program);

    init_shaders(&program, fragment_shaders);

    if (program.error) {
      return program;
    }

    init_textures(&program, context);

    init_input(&program, config, inputs, input_count);

    init_framebuffers(&program, config);

    init_programs(&program, config);

    init_vertices(&program);

    // log_debug("Error after init: %04x",
    //           glGetError()); // TODO check error at each step
  } else {
    program = *previous;
  }

  bind_vertices(&program, previous != NULL ? 1 : 0);

  return program;
}

void shaders_update(ShaderProgram program, File *fragment_shaders,
                    unsigned int i) {
  bool result;

  result = compile_shader(program.fragment_shaders[i], fragment_shaders[i].path,
                          fragment_shaders[i].content);

  if (result) {
    glLinkProgram(program.programs[i]);

    log_info("Program %d updated", i + 1);
  }
}

static void update_viewport(ShaderProgram program, SharedContext *context) {
  unsigned int i;

  // viewport changed
  if (context->width != program.last_width ||
      context->height != program.last_height) {
    // clean and resize all textures
    for (i = 0; i < program.tex_count; i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, context->internal_width,
                   context->internal_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    }
  }
}

static void write_uniform_1f(GLuint location, float value) {
  if (location != unused_uniform) {
    glUniform1f(location, (const GLfloat)value);
  }
}

static void write_uniform_1i(GLuint location, unsigned int value) {
  if (location != unused_uniform) {
    glUniform1i(location, (const GLint)value);
  }
}

static void write_uniform_2f(GLuint location, vec2 *value) {
  if (location != unused_uniform) {
    glUniform2fv(location, 1, (const GLfloat *)value);
  }
}

// static void write_uniform_3f(GLuint location, vec3 *value) {
//   if (location != unused_uniform) {
//     glUniform3fv(location, 1, (const GLfloat *)value);
//   }
// }

static void use_program(ShaderProgram program, int i, bool output,
                        SharedContext *context) {
  unsigned int j, k;
  GLuint subroutines[ARRAY_SIZE];
  vec2 resolution, tex_resolution, in_resolution;

  resolution[0] = (float)context->width;
  resolution[1] = (float)context->height;
  tex_resolution[0] = (float)context->internal_width;
  tex_resolution[1] = (float)context->internal_height;

  // use specific shader program
  glUseProgram(program.programs[i]);

  if (output) {
    glViewport(0, 0, context->width, context->height);

    // use default framebuffer (output)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT);
  } else {
    glViewport(0, 0, context->internal_width, context->internal_height);

    // use memory framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, program.frame_buffers[i]);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
  }
  // set fragment uniforms
  write_uniform_1f(program.itime_locations[i], context->time);
  write_uniform_1f(program.itempo_locations[i], context->tempo);
  write_uniform_1i(program.ifps_locations[i], context->fps);
  write_uniform_1i(program.idemo_locations[i], context->demo ? 1 : 0);
  write_uniform_1i(program.ipage_locations[i], context->page);
  write_uniform_1i(program.iselected_locations[i], context->selected);
  write_uniform_2f(program.ires_locations[i], &resolution);
  write_uniform_2f(program.itexres_locations[i], &tex_resolution);

  for (j = 0; j < program.active_count; j++) {
    write_uniform_1i(program.iactive_locations[i * program.active_count + j],
                     context->active[j] + 1);
  }

  for (j = 0; j < program.in_count; j++) {
    in_resolution[0] = context->input_widths[j];
    in_resolution[1] = context->input_heights[j];

    write_uniform_2f(program.iinres_locations[i * program.in_count + j],
                     &in_resolution);
    write_uniform_1i(program.iinfmt_locations[i * program.in_count + j],
                     context->input_formats[j]);
    write_uniform_1i(program.iinfps_locations[i * program.in_count + j],
                     context->input_fps[j]);
  }

  // set seeds uniforms
  for (j = 0; j < program.frag_count; j++) {
    write_uniform_1i(program.iseed_locations[i * program.frag_count + j],
                     context->seeds[j]);
  }

  // set subroutines for fragment and update state uniforms
  k = context->state[i];
  for (j = 0; j < program.sub_type_count; j++) {
    subroutines[j] = program.sub_locations[i * program.sub_type_count *
                                               program.sub_variant_count +
                                           j * program.sub_variant_count + k];
  }

  for (j = 0; j < program.frag_count; j++) {
    write_uniform_1i(program.istate_locations[i * program.frag_count + j],
                     context->state[j]);
  }

  glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, program.sub_type_count,
                          subroutines);

  // set GL_TEXTURE(X) to uniform sampler2D texX
  for (j = 0; j < program.tex_count; j++) {
    write_uniform_1i(program.textures_locations[i * program.tex_count + j], j);
  }

  // draw output
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void shaders_compute(ShaderProgram program, SharedContext *context,
                     bool monitor, bool output_only) {
  unsigned int i;

  if (!output_only) {
    glBindVertexArray(program.vertex_array[0]);

    update_viewport(program, context);

    for (i = 0; i < program.frag_count; i++) {
      if (i != program.frag_output_index && i != program.frag_monitor_index) {
        use_program(program, i, false, context);
      }
    }
  } else {
    glBindVertexArray(program.vertex_array[1]);

    rebind_textures(&program);
  }

  use_program(program,
              monitor ? program.frag_monitor_index : program.frag_output_index,
              true, context);
}

void shaders_free(ShaderProgram program) {
  unsigned int i;

  for (i = 0; i < program.frag_count; i++) {
    glDeleteProgram(program.programs[i]);
  }

  glDeleteFramebuffers(program.frag_count, program.frame_buffers);
  glDeleteTextures(program.tex_count, program.textures);
  glDeleteBuffers(1, &program.vertex_buffer);
}

void shaders_free_window(ShaderProgram program, bool secondary) {
  glDeleteVertexArrays(1, &program.vertex_array[secondary ? 1 : 0]);
}

void shaders_free_input(ShaderProgram program, VideoCapture input) {
  if (!input.error && input.dma_image != EGL_NO_IMAGE_KHR) {
    eglDestroyImageKHR(program.egl_display, input.dma_image);
  }
}