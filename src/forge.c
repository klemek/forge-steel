#include <log.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "types.h"

#include "config.h"
#include "config_file.h"
#include "file.h"
#include "forge.h"
#include "midi.h"
#include "project.h"
#include "shaders.h"
#include "shared.h"
#include "state.h"
#include "tempo.h"
#include "timer.h"
#include "video.h"
#include "window.h"

static SharedContext *context;
static ShaderProgram program;
static Window *window_output;
static Window *window_monitor;
static VideoCaptureArray inputs;
static Timer timer;
static MidiDevice midi;
static bool trace_midi;
static Project project;

static void compute_fps(bool trace_fps) {
  double fps;
  char title[STR_LEN];

  if (timer_inc(&timer)) {
    fps = timer_reset(&timer);

    if (trace_fps) {
      log_trace("(main) %.2ffps", fps);
    }

    if (window_output != NULL) {
      snprintf(title, STR_LEN, PACKAGE " " VERSION " - %.0ffps", fps);
      window_update_title(window_output, title);
    }

    if (window_monitor != NULL) {
      snprintf(title, STR_LEN, PACKAGE " " VERSION " (monitor) - %.0ffps", fps);
      window_update_title(window_monitor, title);
    }

    context->fps = (unsigned int)round(fps);
  }
}

static void init_context(const Parameters *params) {
  context = shared_init_context("/" PACKAGE "_context");

  context->stop = false;

  state_init(context, &project.state_config, params->demo, params->auto_random,
             params->auto_random_cycle, params->base_tempo, params->load_state);

  memset(context->input_resolutions, 0, sizeof(context->input_resolutions));
  memset(context->input_formats, 0, sizeof(context->input_formats));
  memset(context->input_fps, 0, sizeof(context->input_fps));
}

static void free_context() { shared_close_context(context); }

static void reload_shader(unsigned int i) {
  shaders_update(&program, &project.fragment_shaders[i][0], i, &project);
}

#ifdef VIDEO_IN
static void init_inputs(const StringArray *video_in, unsigned int video_size) {
  inputs.length = video_in->length;

  for (unsigned int i = 0; i < video_in->length; i++) {
    video_init(&inputs.values[i], video_in->values[i], video_size);

    if (!inputs.values[i].error) {
      context->input_resolutions[i][0] = inputs.values[i].width;
      context->input_resolutions[i][1] = inputs.values[i].height;
      context->input_formats[i] = inputs.values[i].pixelformat;
    }
  }
}

static bool start_video_captures(unsigned int video_count, bool trace_fps) {
  for (unsigned int i = 0; i < video_count; i++) {
    if (!inputs.values[i].error &&
        !video_background_read(&inputs.values[i], context, i, trace_fps)) {
      return false;
    }
  }
  return true;
}

static void free_video_captures(unsigned int video_count) {
  for (unsigned int i = 0; i < video_count; i++) {
    shaders_free_input(&program, &inputs.values[i]);

    video_free(&inputs.values[i]);
  }
}
#endif /* VIDEO_IN */

static void error_callback(int error, const char *description) {
  log_error("[GLFW] %d: %s", error, description);
  window_terminate();
  context->stop = true;
  exit(EXIT_FAILURE);
}

static void key_callback(Window *window, int key,
                         __attribute__((unused)) int scancode, int action,
                         int mods) {
  unsigned int event;

  event = window_read_key(key, action, mods);

  if (window_escape_key(key, action)) {
    // close window on escape key
    log_info("[ESC] Closing...");
    window_close(window);
  } else if (event > 0) {
    state_key_event(context, &project.state_config, event, &midi);
  }
}

static void midi_callback(unsigned char code, unsigned char value) {
  state_midi_event(context, &project.state_config, &midi, code, value,
                   trace_midi);
}

static bool init(const Parameters *params) {
  project_init(&project, params->project_path, params->config_file);

  if (project.error) {
    return false;
  }

  init_context(params);

#ifdef VIDEO_IN
  init_inputs(&params->video_in, params->video_size);

  if (!start_video_captures(params->video_in.length, params->trace_fps)) {
    return false;
  }
#endif /* VIDEO_IN */

  midi_open(&midi, config_file_get_str(&project.config, "MIDI_HW", "hw"));

  if (midi.error) {
    context->demo = true;
  } else {
    trace_midi = params->trace_midi;

    if (!midi_background_listen(&midi, context, midi_callback)) {
      return false;
    }
  }

  if (!state_background_write(context, &project.state_config, &midi)) {
    return false;
  }

  window_startup(error_callback);

  context->tex_resolution[1] = params->internal_size;

  if (params->output) {
    window_output = window_init(PACKAGE " " VERSION, params->output_screen,
                                params->windowed, NULL, key_callback);

    window_use(window_output, context);

    shaders_init(&program, &project, context, false);
  } else {
    window_output = NULL;
  }

  if (params->monitor) {
    window_monitor =
        window_init(PACKAGE " " VERSION " (monitor)", params->monitor_screen,
                    params->windowed, window_output, key_callback);

    window_use(window_monitor, context);

    shaders_init(&program, &project, context, window_output != NULL);
  } else {
    window_monitor = NULL;
  }

#ifdef VIDEO_IN
  shaders_link_inputs(&program, &project, &inputs);
#endif /* VIDEO_IN */

  if (program.error) {
    context->stop = true;
    window_terminate();
    exit(EXIT_FAILURE);
  }

  timer_init(&timer, 30);

  log_info("Initialized");

  return true;
}

static bool should_close() {
  return (window_output != NULL && window_should_close(window_output)) ||
         (window_monitor != NULL && window_should_close(window_monitor));
}

static void loop(bool hr, bool trace_fps) {
  if (hr) {
    project_reload(&project, reload_shader);
  }

  compute_fps(trace_fps);

  context->time = window_get_time();
  context->tempo_total = (float)tempo_total(&context->tempo);

  if (window_output != NULL) {
    window_use(window_output, context);

    shaders_compute(&program, context, false, false);

    window_refresh(window_output);
  }

  if (window_monitor != NULL) {
    window_use(window_monitor, context);

    shaders_compute(&program, context, true, window_output != NULL);

    window_refresh(window_monitor);
  }

  window_events();
}

static void shutdown(const Parameters *params) {
  context->stop = true;

  if (params->save_state) {
    state_save(context, &project.state_config);
  }

  shaders_free(&program);

  if (window_output != NULL) {
    window_use(window_output, context);

    shaders_free_window(&program, false);
  }

  if (window_monitor != NULL) {
    window_use(window_monitor, context);

    shaders_free_window(&program, params->output);
  }

#ifdef VIDEO_IN
  free_video_captures(params->video_in.length);
#endif /* VIDEO_IN */

  free_context();

  project_free(&project);

  window_terminate();
}

void forge_run(const Parameters *params) {
  if (!init(params)) {
    return;
  }

  while (!should_close()) {
    loop(params->hot_reload, params->trace_fps);
  }

  shutdown(params);
}