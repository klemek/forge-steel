#include <linmath.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "config.h"
#include "config_file.h"
#include "file.h"
#include "forge.h"
#include "logs.h"
#include "rand.h"
#include "shaders.h"
#include "timer.h"
#include "types.h"
#include "window.h"

static Context context;
static ShaderProgram program;
static ShaderProgram program_monitor;
static Window *window_output;
static Window *window_monitor;

static unsigned int compute_fps(Timer *timer) {
  static double fps;
  char title[100];

  if (timer_inc(timer)) {
    fps = timer_reset(timer);

    if (window_output != NULL) {
      sprintf(title, PACKAGE " " VERSION " - %.0ffps", fps);
      window_update_title(window_output, title);
    }

    if (window_monitor != NULL) {
      sprintf(title, PACKAGE " " VERSION " (monitor) - %.0ffps", fps);
      window_update_title(window_monitor, title);
    }
  }

  return (unsigned int)round(fps);
}

static void randomize_context_state() {
  unsigned int i;
  unsigned int size;
  unsigned int variants;

  size = window_output != NULL
             ? program.frag_count * program.sub_type_count
             : program_monitor.frag_count * program_monitor.sub_type_count;

  variants = window_output != NULL ? program.sub_variant_count
                                   : program_monitor.sub_variant_count;

  for (i = 0; i < size; i++) {
    context.sub_state[i] = rand_uint(variants);
  }
}
static void init_context(Parameters params) {
  unsigned int size;
  unsigned int i;

  context.tempo = params.base_tempo;
  context.demo = params.demo;
  context.monitor = params.monitor;

  size = window_output != NULL
             ? program.frag_count * program.sub_type_count
             : program_monitor.frag_count * program_monitor.sub_type_count;
  context.sub_state = malloc(size * sizeof(unsigned int));

  for (i = 0; i < size; i++) {
    context.sub_state[i] = 0;
  }

  if (params.demo) {
    randomize_context_state();
  }

  size =
      window_output != NULL ? program.frag_count : program_monitor.frag_count;
  context.seeds = malloc(size * sizeof(unsigned int));
  for (i = 0; i < size; i++) {
    context.seeds[i] = rand_uint(1000);
  }
}

static void free_context() {
  free(context.sub_state);
  free(context.seeds);
}

static void hot_reload(File *common_shader_code, File *fragment_shaders) {
  unsigned int i;
  unsigned int frag_count;
  bool force_update;

  force_update = false;

  frag_count =
      window_output != NULL ? program.frag_count : program_monitor.frag_count;

  if (file_should_update(*common_shader_code)) {
    file_update(common_shader_code);
    force_update = true;
  }

  for (i = 0; i < frag_count; i++) {
    if (force_update || file_should_update(fragment_shaders[i])) {
      file_update(&fragment_shaders[i]);
      file_prepend(&fragment_shaders[i], *common_shader_code);

      if (window_output != NULL) {
        shaders_update(program, fragment_shaders, i);
      }

      if (window_monitor != NULL) {
        shaders_update(program_monitor, fragment_shaders, i);
      }
    }
  }
}

static void loop(bool hr, File *common_shader_code, File *fragment_shaders,
                 unsigned int monitor_factor, Timer *timer) {
  if (hr) {
    hot_reload(common_shader_code, fragment_shaders);
  }

  context.fps = compute_fps(timer);

  context.time = window_get_time();

  if (window_output != NULL) {
    window_use(window_output, &context);

    shaders_compute(program, context, false, 1);

    window_refresh(window_output);
  }

  if (window_monitor != NULL) {
    window_use(window_monitor, &context);

    shaders_compute(program_monitor, context, true, monitor_factor);

    window_refresh(window_monitor);
  }

  window_events();
}

File read_fragment_shader_file(char *frag_path, unsigned int i) {
  File fragment_shader;
  char *file_path;

  file_path = malloc(sizeof(char) * 1024);

  sprintf(file_path, "%s/frag%d.glsl", frag_path, i);
  fragment_shader = file_read(file_path);
  if (fragment_shader.error) {
    exit(EXIT_FAILURE);
  }

  return fragment_shader;
}

static void init_files(char *frag_path, File *common_shader_code,
                       File *fragment_shaders, unsigned int frag_count) {
  unsigned int i;

  for (i = 0; i < frag_count + 1; i++) {
    if (i == 0) {
      (*common_shader_code) = read_fragment_shader_file(frag_path, i);
    } else {
      fragment_shaders[i - 1] = read_fragment_shader_file(frag_path, i);

      file_prepend(&fragment_shaders[i - 1], *common_shader_code);
    }
  }
}

static void free_files(File *common_shader_code, File *fragment_shaders,
                       unsigned int frag_count) {
  unsigned int i;

  for (i = 0; i < frag_count; i++) {
    file_free(&fragment_shaders[i], true);
  }

  file_free(common_shader_code, true);
}

static void error_callback(int error, const char *description) {
  log_error("[GLFW] %d: %s", error, description);
  window_close(0, true);
  exit(EXIT_FAILURE);
}

static void key_callback(Window *window, int key,
                         __attribute__((unused)) int scancode, int action,
                         __attribute__((unused)) int mods) {
  if (window_escape_key(key, action)) {
    // close window on escape key
    window_close(window, false);
  } else if (window_char_key(key, action, 82)) {
    // R: randomize
    randomize_context_state();
  } else if (window_char_key(key, action, 68)) {
    // D: demo on/off
    context.demo = !context.demo;
  }
}

void forge_run(Parameters params) {
  unsigned int frag_count;
  File *fragment_shaders;
  File common_shader_code;
  Timer timer;
  ConfigFile shader_config;

  shader_config = config_file_read(params.frag_config_path, false);

  frag_count = config_file_get_int(shader_config, "FRAG_COUNT", 6);

  fragment_shaders = malloc(frag_count * sizeof(File));

  init_files(params.frag_path, &common_shader_code, fragment_shaders,
             frag_count);

  window_startup(error_callback);

  context.internal_size = params.internal_size;

  if (params.output) {
    window_output = window_init(PACKAGE " " VERSION, params.output_screen,
                                params.windowed, NULL, key_callback);

    window_use(window_output, &context);

    program = shaders_init(fragment_shaders, shader_config, context, 1);
  } else {
    window_output = NULL;
  }

  if (params.monitor) {
    window_monitor =
        window_init(PACKAGE " " VERSION " (monitor)", params.monitor_screen,
                    params.windowed, window_output, key_callback);

    window_use(window_monitor, &context);

    program_monitor = shaders_init(fragment_shaders, shader_config, context,
                                   params.monitor_factor);
  } else {
    window_monitor = NULL;
  }

  init_context(params);

  if ((window_output != NULL && program.error) ||
      (window_monitor != NULL && program_monitor.error)) {
    if (window_output != NULL) {
      window_close(window_output, true);
    }
    if (window_monitor != NULL) {
      window_close(window_monitor, true);
    }
    exit(EXIT_FAILURE);
  }

  timer = timer_init(30);

  log_success("Initialized");

  while ((window_output == NULL || !window_should_close(window_output)) &&
         (window_monitor == NULL || !window_should_close(window_monitor))) {
    loop(params.hot_reload, &common_shader_code, fragment_shaders,
         params.monitor_factor, &timer);
  }

  free_files(&common_shader_code, fragment_shaders, frag_count);

  config_file_free(shader_config);

  if (window_output != NULL) {
    window_close(window_output, true);
  }
  if (window_monitor != NULL) {
    window_close(window_monitor, true);
  }

  free_context();
}