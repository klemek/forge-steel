#include <linmath.h>
#include <stdbool.h>
#include <stdlib.h>

#include "config.h"
#include "file.h"
#include "logs.h"
#include "shaders.h"
#include "timer.h"
#include "types.h"
#include "window.h"

void error_callback(int error, const char *description) {
  log_error("[GLFW] %d: %s", error, description);
  close_window(0, true);
  exit(EXIT_FAILURE);
}

static void key_callback(Window *window, int key,
                         __attribute__((unused)) int scancode, int action,
                         __attribute__((unused)) int mods) {
  // close window on escape key
  if (escape_key(key, action)) {
    close_window(window, false);
  }
}

void compute_fps(Window *window, Timer *timer) {
  double fps;
  char title[100];

  if (inc_timer(timer)) {
    fps = reset_and_count(timer);
    sprintf(title, PACKAGE " " VERSION " - %.0ffps", fps);
    update_window_title(window, title);
  }
}

void loop(Window *window, ShaderProgram program, bool hot_reload,
          File *fragment_shaders, Timer *timer) {
  Context context;
  int i;

  compute_fps(window, timer);

  if (hot_reload) {
    // TODO extract to function
    for (i = 0; i < FRAG_COUNT; i++) {
      if (should_update_file(fragment_shaders[i])) {
        update_file(&fragment_shaders[i]);
        update_program(program, fragment_shaders, i);
      }
    }
  }

  context = get_window_context(window);

  apply_program(program, context);

  refresh_window(window);
}

void forge_run(Parameters params) {
  File fragment_shaders[FRAG_COUNT];
  ShaderProgram program;
  Window *window;
  Timer timer;
  Context context;
  int i;
  char file_path[FRAG_COUNT][1024];

  // TODO extract to function
  for (i = 0; i < FRAG_COUNT; i++) {
    sprintf(file_path[i], "%s/frag%d.glsl", params.frag_path, i + 1);
    fragment_shaders[i] = read_file(file_path[i]);
    if (fragment_shaders[i].error) {
      exit(EXIT_FAILURE);
    }
  }

  window = init_window(PACKAGE " " VERSION, params.screen, error_callback,
                       key_callback);

  context = get_window_context(window);

  program = init_program(fragment_shaders, context);

  if (program.error) {
    close_window(window, true);
    exit(EXIT_FAILURE);
  }

  timer = create_timer(60);

  while (!window_should_close(window)) {
    loop(window, program, params.hot_reload, fragment_shaders, &timer);
  }

  close_window(window, true);

  // TODO extract to function
  for (i = 0; i < FRAG_COUNT; i++) {
    free_file(&fragment_shaders[i]);
  }
}