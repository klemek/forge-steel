#include <linmath.h>
#include <math.h>
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

int compute_fps(Window *window, Timer *timer) {
  static double fps;
  char title[100];

  if (inc_timer(timer)) {
    fps = reset_and_count(timer);
    sprintf(title, PACKAGE " " VERSION " - %.0ffps", fps);
    update_window_title(window, title);
  }

  return (int)round(fps);
}

void hot_reload(ShaderProgram program, File *common_shader_code,
                File *fragment_shaders) {
  int i;
  bool force_update = false;

  if (should_update_file(*common_shader_code)) {
    update_file(common_shader_code);
    force_update = true;
  }

  for (i = 0; i < FRAG_COUNT; i++) {
    if (force_update || should_update_file(fragment_shaders[i])) {
      update_file(&fragment_shaders[i]);
      prepend_file(&fragment_shaders[i], *common_shader_code);
      update_program(program, fragment_shaders, i);
    }
  }
}

void loop(Window *window, ShaderProgram program, bool hr,
          File *common_shader_code, File *fragment_shaders, Timer *timer) {
  Context context;

  if (hr) {
    hot_reload(program, common_shader_code, fragment_shaders);
  }

  context = get_window_context(window);

  context.fps = compute_fps(window, timer);

  apply_program(program, context);

  refresh_window(window);
}

File read_fragment_shader_file(char *frag_path, int i) {
  File fragment_shader;
  char *file_path = malloc(sizeof(char) * 1024);

  sprintf(file_path, "%s/frag%d.glsl", frag_path, i);
  fragment_shader = read_file(file_path);
  if (fragment_shader.error) {
    exit(EXIT_FAILURE);
  }

  return fragment_shader;
}

void init_files(char *frag_path, File *common_shader_code,
                File *fragment_shaders) {
  int i;

  for (i = 0; i < FRAG_COUNT + 1; i++) {
    if (i == 0) {
      (*common_shader_code) = read_fragment_shader_file(frag_path, i);
    } else {
      fragment_shaders[i - 1] = read_fragment_shader_file(frag_path, i);

      prepend_file(&fragment_shaders[i - 1], *common_shader_code);
    }
  }
}

void free_files(File *common_shader_code, File *fragment_shaders) {
  int i;

  for (i = 0; i < FRAG_COUNT; i++) {
    free_file(&fragment_shaders[i]);
  }

  free_file(common_shader_code);
}

void forge_run(Parameters params) {
  File fragment_shaders[FRAG_COUNT];
  File common_shader_code;
  ShaderProgram program;
  Window *window;
  Timer timer;
  Context context;

  init_files(params.frag_path, &common_shader_code, fragment_shaders);

  window = init_window(PACKAGE " " VERSION, params.screen, error_callback,
                       key_callback);

  context = get_window_context(window);

  program = init_program(fragment_shaders, context);

  if (program.error) {
    close_window(window, true);
    exit(EXIT_FAILURE);
  }

  timer = create_timer(30);

  while (!window_should_close(window)) {
    loop(window, program, params.hot_reload, &common_shader_code,
         fragment_shaders, &timer);
  }

  close_window(window, true);

  free_files(&common_shader_code, fragment_shaders);
}