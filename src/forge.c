#include <log.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "config.h"
#include "config_file.h"
#include "file.h"
#include "forge.h"
#include "midi.h"
#include "rand.h"
#include "shaders.h"
#include "shared.h"
#include "timer.h"
#include "types.h"
#include "video.h"
#include "window.h"

static SharedContext *context;
static ShaderProgram program;
static Window *window_output;
static Window *window_monitor;
static VideoCapture *inputs;
static File *fragment_shaders;
static File common_shader_code;
static Timer timer;
static ConfigFile config;
static MidiDevice midi;

static void compute_fps() {
  double fps;
  char title[100];

  if (timer_inc(&timer)) {
    fps = timer_reset(&timer);

    log_trace("(main) %.2ffps", fps);

    if (window_output != NULL) {
      sprintf(title, PACKAGE " " VERSION " - %.0ffps", fps);
      window_update_title(window_output, title);
    }

    if (window_monitor != NULL) {
      sprintf(title, PACKAGE " " VERSION " (monitor) - %.0ffps", fps);
      window_update_title(window_monitor, title);
    }

    context->fps = (unsigned int)round(fps);
  }
}

static void randomize_context_state() {
  unsigned int i;

  for (i = 0; i < program.frag_count * program.sub_type_count; i++) {
    context->sub_state[i] = rand_uint(program.sub_variant_count);
  }
}
static void init_context(Parameters params) {
  unsigned int i;

  context->tempo = params.base_tempo;
  context->demo = params.demo;
  context->monitor = params.monitor;

  memset(context->sub_state, 0, sizeof(context->sub_state));

  if (params.demo) {
    randomize_context_state();
  }

  memset(context->seeds, 0, sizeof(context->seeds));

  for (i = 0; i < program.frag_count; i++) {
    context->seeds[i] = rand_uint(1000);
  }

  memset(context->input_widths, 0, sizeof(context->input_widths));
  memset(context->input_heights, 0, sizeof(context->input_heights));
  memset(context->input_formats, 0, sizeof(context->input_formats));
  memset(context->input_fps, 0, sizeof(context->input_fps));

  for (i = 0; i < program.in_count; i++) {
    if (!inputs[i].error) {
      context->input_widths[i] = inputs[i].width;
      context->input_heights[i] = inputs[i].height;
      context->input_formats[i] = inputs[i].pixelformat;
    }
  }
}

static void free_context() { shared_close_context(context); }

static void hot_reload() {
  unsigned int i;
  bool force_update;

  force_update = false;

  if (file_should_update(common_shader_code)) {
    file_update(&common_shader_code);
    force_update = true;
  }

  for (i = 0; i < program.frag_count; i++) {
    if (force_update || file_should_update(fragment_shaders[i])) {
      file_update(&fragment_shaders[i]);
      file_prepend(&fragment_shaders[i], common_shader_code);

      shaders_update(program, fragment_shaders, i);
    }
  }
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

static void init_files(char *frag_path, unsigned int frag_count) {
  unsigned int i;

  fragment_shaders = malloc(frag_count * sizeof(File));

  for (i = 0; i < frag_count + 1; i++) {
    if (i == 0) {
      common_shader_code = read_fragment_shader_file(frag_path, i);
    } else {
      fragment_shaders[i - 1] = read_fragment_shader_file(frag_path, i);

      file_prepend(&fragment_shaders[i - 1], common_shader_code);
    }
  }
}

static void free_files(unsigned int frag_count) {
  unsigned int i;

  for (i = 0; i < frag_count; i++) {
    file_free(&fragment_shaders[i], true);
  }

  file_free(&common_shader_code, true);
}

static void init_inputs(char *video_in[MAX_VIDEO], unsigned int input_count,
                        unsigned int video_size) {
  unsigned int i;

  inputs = malloc(input_count * sizeof(VideoCapture));

  for (i = 0; i < input_count; i++) {
    inputs[i] = video_init(video_in[i], video_size);
  }
}

static bool start_video_captures(unsigned int video_count) {
  unsigned int i;

  for (i = 0; i < video_count; i++) {
    if (!inputs[i].error && !video_background_read(&inputs[i], context, i)) {
      return false;
    }
  }

  return true;
}

static void free_video_captures(unsigned int video_count) {
  unsigned int i;

  for (i = 0; i < video_count; i++) {
    shaders_free_input(program, inputs[i]);

    video_free(inputs[i]);
  }

  free(inputs);
}

static void error_callback(int error, const char *description) {
  log_error("[GLFW] %d: %s", error, description);
  window_terminate();
  context->stop = true;
  exit(EXIT_FAILURE);
}

static void key_callback(Window *window, int key,
                         __attribute__((unused)) int scancode, int action,
                         __attribute__((unused)) int mods) {
  if (window_escape_key(key, action)) {
    // close window on escape key
    window_close(window);
  } else if (window_char_key(key, action, 82)) {
    // R: randomize
    randomize_context_state();
  } else if (window_char_key(key, action, 68)) {
    // D: demo on/off
    context->demo = !context->demo;
  }
}

static void loop(bool hr) {
  if (hr) {
    hot_reload();
  }

  compute_fps();

  context->time = window_get_time();

  if (window_output != NULL) {
    window_use(window_output, context);

    shaders_compute(program, context, false, false);

    window_refresh(window_output);
  }

  if (window_monitor != NULL) {
    window_use(window_monitor, context);

    shaders_compute(program, context, true, window_output != NULL);

    window_refresh(window_monitor);
  }

  window_events();
}

void forge_run(Parameters params) {
  unsigned int frag_count;

  context = shared_init_context("/" PACKAGE "_context");

  context->stop = false;

  config = config_file_read(params.config_path, false);

  frag_count = config_file_get_int(config, "FRAG_COUNT", 6);

  init_files(params.frag_path, frag_count);

  init_inputs(params.video_in, params.video_in_count, params.video_size);

  if (!start_video_captures(params.video_in_count)) {
    return;
  }

  midi = midi_open(config_file_get_str(config, "MIDI_HW", "hw"));

  if (midi.error) {
    params.demo = true;
  } else {
    if (!midi_background_listen(midi, context)) {
      return;
    }
  }

  window_startup(error_callback);

  context->internal_height = params.internal_size;

  if (params.output) {
    window_output = window_init(PACKAGE " " VERSION, params.output_screen,
                                params.windowed, NULL, key_callback);

    window_use(window_output, context);

    program = shaders_init(fragment_shaders, config, context, inputs,
                           params.video_in_count, NULL);
  } else {
    window_output = NULL;
  }

  if (params.monitor) {
    window_monitor =
        window_init(PACKAGE " " VERSION " (monitor)", params.monitor_screen,
                    params.windowed, window_output, key_callback);

    window_use(window_monitor, context);

    program = shaders_init(fragment_shaders, config, context, inputs,
                           params.video_in_count,
                           window_output != NULL ? &program : NULL);
  } else {
    window_monitor = NULL;
  }

  init_context(params);

  if (program.error) {
    window_terminate();
    exit(EXIT_FAILURE);
  }

  timer = timer_init(30);

  log_info("Initialized");

  while ((window_output == NULL || !window_should_close(window_output)) &&
         (window_monitor == NULL || !window_should_close(window_monitor))) {
    loop(params.hot_reload);
  }

  context->stop = true;

  shaders_free(program);

  if (window_output != NULL) {
    window_use(window_output, context);

    shaders_free_window(program, false);
  }

  if (window_monitor != NULL) {
    window_use(window_monitor, context);

    shaders_free_window(program, params.output);
  }

  free_video_captures(params.video_in_count);

  free_context();

  free_files(frag_count);

  config_file_free(config);

  window_terminate();
}