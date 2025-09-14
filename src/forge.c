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
          File *fragment_shader, Timer *timer) {
  Context context;

  compute_fps(window, timer);

  if (hot_reload && should_update_file(*fragment_shader)) {
    update_file(fragment_shader);
    update_program(program, *fragment_shader);
  }

  context = get_window_context(window);

  apply_program(program, context);

  refresh_window(window);
}

void forge_run(Parameters params) {
  File fragment_shader;
  ShaderProgram program;
  Window *window;
  Timer timer;
  Context context;

  fragment_shader = read_file(params.frag_path);

  if (fragment_shader.error) {
    exit(EXIT_FAILURE);
  }

  window = init_window(PACKAGE " " VERSION, params.screen, error_callback,
                       key_callback);

  context = get_window_context(window);

  program = init_program(fragment_shader, context);

  if (program.error) {
    close_window(window, true);
    exit(EXIT_FAILURE);
  }

  timer = create_timer(60);

  while (!window_should_close(window)) {
    loop(window, program, params.hot_reload, &fragment_shader, &timer);
  }

  close_window(window, true);

  free_file(&fragment_shader);
}