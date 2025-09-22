#include <linmath.h>
#include <log.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

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

static void init_textures(ShaderProgram *program, Context context) {
  unsigned int i;

  program->textures = malloc(program->tex_count * sizeof(GLuint));

  glGenTextures(program->tex_count, program->textures);

  for (i = 0; i < program->tex_count; i++) {
    // selects which texture unit subsequent texture state calls will affect
    glActiveTexture(GL_TEXTURE0 + i);

    glBindTexture(GL_TEXTURE_2D, program->textures[i]);

    glEnable(GL_TEXTURE_2D);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    // define texture image as empty
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, context.internal_width,
                 context.internal_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

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

static void link_video_to_texture(ShaderProgram *program, VideoDevice *device,
                                  unsigned int texture_index) {
  device->dma_image = EGL_NO_IMAGE_KHR;

  const EGLint attrib_list[] = {EGL_WIDTH,
                                device->width,
                                EGL_HEIGHT,
                                device->height,
                                EGL_LINUX_DRM_FOURCC_EXT,
                                device->pixelformat,
                                EGL_DMA_BUF_PLANE0_FD_EXT,
                                device->exp_fd,
                                EGL_DMA_BUF_PLANE0_OFFSET_EXT,
                                0,
                                EGL_DMA_BUF_PLANE0_PITCH_EXT,
                                device->bytesperline,
                                EGL_NONE};

  device->dma_image = eglCreateImageKHR(program->egl_display, EGL_NO_CONTEXT,
                                        EGL_LINUX_DMA_BUF_EXT,
                                        (EGLClientBuffer)NULL, attrib_list);

  if (device->dma_image == EGL_NO_IMAGE_KHR) {
    log_error("(%s) eglCreateImageKHR failed %04x", device->name,
              eglGetError());
    return;
  }

  glActiveTexture(GL_TEXTURE0 + texture_index);

  glBindTexture(GL_TEXTURE_2D, program->textures[texture_index]);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, device->width, device->height, 0,
               GL_RGB, GL_UNSIGNED_BYTE, 0);

  // https://registry.khronos.org/OpenGL/extensions/EXT/EXT_EGL_image_storage.txt
  glEGLImageTargetTextureStorageEXT(program->textures[texture_index],
                                    (GLeglImageOES)device->dma_image, NULL);

  log_info("Texture %d linked to %s", texture_index, device->name);
}

static void init_videos(ShaderProgram *program, ConfigFile shader_config,
                        VideoDevice *devices, unsigned int device_count) {
  unsigned int i;
  unsigned tex_i;
  char name[32];

  for (i = 0; i < program->in_count; i++) {
    if (i < device_count && !devices[i].error) {
      sprintf(name, "IN_%d_OUT", i + 1);
      tex_i = config_file_get_int(shader_config, name, 0);
      link_video_to_texture(program, &devices[i], tex_i);
    } else {
      log_warn("Cannot link input %d", i + 1);
    }
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
  char *prefix;
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
  program->itexres_locations[i] = glGetUniformLocation(
      program->programs[i],
      config_file_get_str(shader_config, "UNIFORM_TEX_RESOLUTION",
                          "iTexResolution"));
  program->idemo_locations[i] = glGetUniformLocation(
      program->programs[i],
      config_file_get_str(shader_config, "UNIFORM_DEMO", "iDemo"));

  prefix = config_file_get_str(shader_config, "UNIFORM_IN_RESOLUTION_PREFIX",
                               "iInputResolution");

  for (j = 0; j < program->in_count; j++) {
    sprintf(name, "%s%d", prefix, j + 1);
    program->iinres_locations[i * program->in_count + j] =
        glGetUniformLocation(program->programs[i], name);
  }

  prefix = config_file_get_str(shader_config, "UNIFORM_SEED_PREFIX", "seed");
  for (j = 0; j < program->frag_count; j++) {
    sprintf(name, "%s%d", prefix, j + 1);
    program->iseed_locations[i * program->frag_count + j] =
        glGetUniformLocation(program->programs[i], name);
  }

  prefix = config_file_get_str(shader_config, "UNIFORM_STATE_PREFIX", "state");
  for (j = 0; j < program->frag_count; j++) {
    for (k = 0; k < program->sub_type_count; k++) {
      sprintf(name, "%s%d_%d", prefix, j + 1, k + 1);
      program
          ->istate_locations[i * program->frag_count * program->sub_type_count +
                             j * program->sub_type_count + k] =
          glGetUniformLocation(program->programs[i], name);
    }
  }

  for (j = 0; j < program->sub_type_count; j++) {
    sprintf(name, "SUB_%d_PREFIX", j + 1);
    prefix = config_file_get_str(shader_config, name, 0);
    for (k = 0; k < program->sub_variant_count; k++) {
      sprintf(name, "%s%d", prefix, k + 1);
      program->sub_locations[i * program->sub_variant_count *
                                 program->sub_type_count +
                             j * program->sub_variant_count + k] =
          glGetSubroutineIndex(program->programs[i], GL_FRAGMENT_SHADER, name);
    }
  }

  // create texX uniforms pointer
  prefix = config_file_get_str(shader_config, "UNIFORM_TEX_PREFIX", "tex");
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

static void init_programs(ShaderProgram *program, ConfigFile shader_config) {
  unsigned int i;

  program->programs = malloc(program->frag_count * sizeof(GLuint));
  program->itime_locations = malloc(program->frag_count * sizeof(GLuint));
  program->itempo_locations = malloc(program->frag_count * sizeof(GLuint));
  program->ifps_locations = malloc(program->frag_count * sizeof(GLuint));
  program->ires_locations = malloc(program->frag_count * sizeof(GLuint));
  program->itexres_locations = malloc(program->frag_count * sizeof(GLuint));
  program->iinres_locations =
      malloc(program->frag_count * program->in_count * sizeof(GLuint));
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
                           Context context, VideoDevice *devices,
                           unsigned int device_count, ShaderProgram *previous) {
  ShaderProgram program;

  if (previous == NULL) {
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
        config_file_get_int(shader_config, "SUB_VARIANT_COUNT", 0);
    program.in_count = config_file_get_int(shader_config, "IN_COUNT", 0);

    init_gl(&program);

    init_shaders(&program, fragment_shaders);

    if (program.error) {
      return program;
    }

    init_textures(&program, context);

    init_videos(&program, shader_config, devices, device_count);

    init_framebuffers(&program, shader_config);

    init_programs(&program, shader_config);

    init_vertices(&program);

    log_debug("Error after init: %04x",
              glGetError()); // TODO check error at each step
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

static void update_viewport(ShaderProgram program, Context context) {
  unsigned int i;

  // viewport changed
  if (context.width != program.last_width ||
      context.height != program.last_height) {
    // clean and resize all textures
    for (i = 0; i < program.tex_count; i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, context.internal_width,
                   context.internal_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    }
  }
}

static void use_program(ShaderProgram program, int i, bool output,
                        Context context) {
  unsigned int j, k;
  GLuint *subroutines;
  vec2 resolution, tex_resolution;

  resolution[0] = (float)context.width;
  resolution[1] = (float)context.height;
  tex_resolution[0] = (float)context.internal_width;
  tex_resolution[1] = (float)context.internal_height;
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
    glViewport(0, 0, context.internal_width, context.internal_height);

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
  glUniform2fv(program.itexres_locations[i], 1,
               (const GLfloat *)&tex_resolution);

  // TODO video resolution

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

  free(subroutines);
}

void shaders_compute(ShaderProgram program, Context context, bool monitor,
                     bool output_only) {
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

void shaders_free_video(ShaderProgram program, VideoDevice device) {
  if (!device.error && device.dma_image != EGL_NO_IMAGE_KHR) {
    eglDestroyImageKHR(program.egl_display, device.dma_image);
  }
}