#include <log.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "config.h"
#include "config_file.h"
#include "file.h"
#include "forge.h"
#include "midi.h"
#include "shaders.h"
#include "shared.h"
#include "state.h"
#include "tempo.h"
#include "timer.h"
#include "types.h"
#include "video.h"
#include "window.h"

static SharedContext *context;
static ShaderProgram program;
static Window *window_output;
static Window *window_monitor;
static VideoCaptureArray inputs;
static FileArray fragment_shaders;
static File common_shader_code;
static Timer timer;
static ConfigFile config;
static MidiDevice midi;
static StateConfig state_config;
static bool trace_midi;

static void compute_fps(bool trace_fps) {
  double fps;
  char title[STR_LEN];

  if (timer_inc(&timer)) {
    fps = timer_reset(&timer);

    if (trace_fps) {
      log_trace("(main) %.2ffps", fps);
    }

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

static void init_context(Parameters params, unsigned int in_count) {
  unsigned int i;

  state_init(context, state_config, params.demo, params.auto_random,
             params.base_tempo, params.state_file, params.load_state);

  context->monitor = params.monitor;

  memset(context->input_resolutions, 0, sizeof(context->input_resolutions));
  memset(context->input_formats, 0, sizeof(context->input_formats));
  memset(context->input_fps, 0, sizeof(context->input_fps));

  for (i = 0; i < in_count; i++) {
    if (!inputs.values[i].error) {
      context->input_resolutions[i][0] = inputs.values[i].width;
      context->input_resolutions[i][1] = inputs.values[i].height;
      context->input_formats[i] = inputs.values[i].pixelformat;
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
    if (force_update || file_should_update(fragment_shaders.values[i])) {
      file_update(&fragment_shaders.values[i]);
      file_prepend(&fragment_shaders.values[i], common_shader_code);

      shaders_update(program, fragment_shaders, i);
    }
  }
}

File read_fragment_shader_file(char *frag_path, char *frag_prefix,
                               unsigned int i) {
  File fragment_shader;
  char file_path[STR_LEN];

  snprintf(file_path, STR_LEN, "%s/%s%d.glsl", frag_path, frag_prefix, i);
  fragment_shader = file_read(file_path);
  if (fragment_shader.error) {
    exit(EXIT_FAILURE);
  }

  return fragment_shader;
}

static void init_files(char *frag_path, char *frag_prefix,
                       unsigned int frag_count) {
  unsigned int i;

  fragment_shaders.length = frag_count;

  for (i = 0; i < frag_count + 1; i++) {
    if (i == 0) {
      common_shader_code = read_fragment_shader_file(frag_path, frag_prefix, i);
    } else {
      fragment_shaders.values[i - 1] =
          read_fragment_shader_file(frag_path, frag_prefix, i);

      file_prepend(&fragment_shaders.values[i - 1], common_shader_code);
    }
  }
}

static void free_files(unsigned int frag_count) {
  unsigned int i;

  for (i = 0; i < frag_count; i++) {
    file_free(&fragment_shaders.values[i]);
  }

  file_free(&common_shader_code);
}

static void init_inputs(StringArray video_in, unsigned int video_size) {
  unsigned int i;

  inputs.length = video_in.length;

  for (i = 0; i < video_in.length; i++) {
    inputs.values[i] = video_init(video_in.values[i], video_size);
  }
}

static bool start_video_captures(unsigned int video_count, bool trace_fps) {
  unsigned int i;

  for (i = 0; i < video_count; i++) {
    if (!inputs.values[i].error &&
        !video_background_read(&inputs.values[i], context, i, trace_fps)) {
      return false;
    }
  }

  return true;
}

static void free_video_captures(unsigned int video_count) {
  unsigned int i;

  for (i = 0; i < video_count; i++) {
    shaders_free_input(program, inputs.values[i]);

    video_free(inputs.values[i]);
  }
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
    log_info("[ESC] Closing...");
    window_close(window);
  } else if (window_char_key(key, action, 82)) {
    // R: randomize
    log_info("[R] Randomizing...");
    state_randomize(context, state_config);
  } else if (window_char_key(key, action, 68)) {
    // D: demo on/off
    log_info((context->demo ? "[D] Demo OFF" : "[D] Demo ON"));
    context->demo = !context->demo;
  } else if (window_char_key(key, action, 65)) {
    // A: auto random on/off
    log_info(
        (context->auto_random ? "[A] Auto Random OFF" : "[A] Auto Random ON"));
    context->auto_random = !context->auto_random;
  }
}

static void midi_callback(unsigned char code, unsigned char value) {
  state_apply_event(context, state_config, midi, code, value, trace_midi);
}

static void loop(bool hr, bool trace_fps) {
  if (hr) {
    hot_reload();
  }

  compute_fps(trace_fps);

  context->time = window_get_time();
  context->tempo_total = (float)tempo_total(context->tempo);

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
  unsigned int frag_count, in_count;
  char config_path[STR_LEN * 2 + 1];
  char *frag_prefix;

  context = shared_init_context("/" PACKAGE "_context");

  context->stop = false;

  sprintf(config_path, "%s/%s", params.project_path, params.config_file);

  config = config_file_read(config_path);

  state_config = state_parse_config(config);

  frag_count = config_file_get_int(config, "FRAG_COUNT", 1);
  in_count = config_file_get_int(config, "IN_COUNT", 0);
  frag_prefix = config_file_get_str(config, "FRAG_FILE_PREFIX", "frag");

  init_files(params.project_path, frag_prefix, frag_count);

  init_inputs(params.video_in, params.video_size);

  init_context(params, in_count);

  if (!start_video_captures(params.video_in.length, params.trace_fps)) {
    return;
  }

  midi = midi_open(config_file_get_str(config, "MIDI_HW", "hw"));

  if (midi.error) {
    params.demo = true;
  } else {
    trace_midi = params.trace_midi;

    if (!midi_background_listen(midi, context, midi_callback)) {
      return;
    }
  }

  if (!state_background_write(context, state_config, midi)) {
    return;
  }

  window_startup(error_callback);

  context->tex_resolution[1] = params.internal_size;

  if (params.output) {
    window_output = window_init(PACKAGE " " VERSION, params.output_screen,
                                params.windowed, NULL, key_callback);

    window_use(window_output, context);

    program = shaders_init(fragment_shaders, config, context, inputs,
                           state_config, NULL);
  } else {
    window_output = NULL;
  }

  if (params.monitor) {
    window_monitor =
        window_init(PACKAGE " " VERSION " (monitor)", params.monitor_screen,
                    params.windowed, window_output, key_callback);

    window_use(window_monitor, context);

    program =
        shaders_init(fragment_shaders, config, context, inputs, state_config,
                     window_output != NULL ? &program : NULL);
  } else {
    window_monitor = NULL;
  }

  if (program.error) {
    context->stop = true;
    window_terminate();
    exit(EXIT_FAILURE);
  }

  timer = timer_init(30);

  log_info("Initialized");

  while ((window_output == NULL || !window_should_close(window_output)) &&
         (window_monitor == NULL || !window_should_close(window_monitor))) {
    loop(params.hot_reload, params.trace_fps);
  }

  context->stop = true;

  if (params.save_state) {
    state_save(context, state_config, params.state_file);
  }

  shaders_free(program);

  if (window_output != NULL) {
    window_use(window_output, context);

    shaders_free_window(program, false);
  }

  if (window_monitor != NULL) {
    window_use(window_monitor, context);

    shaders_free_window(program, params.output);
  }

  free_video_captures(params.video_in.length);

  free_context();

  free_files(frag_count);

  config_file_free(config);

  window_terminate();
}