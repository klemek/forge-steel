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

static void error_callback(int error, const char *description) {
  log_error("[GLFW] %d: %s", error, description);
  window_close(0, true);
  exit(EXIT_FAILURE);
}

static void key_callback(Window *window, int key,
                         __attribute__((unused)) int scancode, int action,
                         __attribute__((unused)) int mods) {
  // close window on escape key
  if (window_escape_key(key, action)) {
    window_close(window, false);
  }
}

static int compute_fps(Window *window, Timer *timer) {
  static double fps;
  char title[100];

  if (timer_inc(timer)) {
    fps = timer_reset(timer);
    sprintf(title, PACKAGE " " VERSION " - %.0ffps", fps);
    window_update_title(window, title);
  }

  return (int)round(fps);
}

static void hot_reload(ShaderProgram program, File *common_shader_code,
                       File *fragment_shaders) {
  int i;
  bool force_update = false;

  if (file_should_update(*common_shader_code)) {
    file_update(common_shader_code);
    force_update = true;
  }

  for (i = 0; i < FRAG_COUNT; i++) {
    if (force_update || file_should_update(fragment_shaders[i])) {
      file_update(&fragment_shaders[i]);
      file_prepend(&fragment_shaders[i], *common_shader_code);
      shaders_update(program, fragment_shaders, i);
    }
  }
}

static void loop(Window *window, ShaderProgram program, bool hr,
                 File *common_shader_code, File *fragment_shaders,
                 Timer *timer) {
  Context context;

  if (hr) {
    hot_reload(program, common_shader_code, fragment_shaders);
  }

  context = window_get_context(window);

  context.fps = compute_fps(window, timer);

  shaders_apply(program, context);

  window_refresh(window);
}

File read_fragment_shader_file(char *frag_path, int i) {
  File fragment_shader;
  char *file_path = malloc(sizeof(char) * 1024);

  sprintf(file_path, "%s/frag%d.glsl", frag_path, i);
  fragment_shader = file_read(file_path);
  if (fragment_shader.error) {
    exit(EXIT_FAILURE);
  }

  return fragment_shader;
}

static void init_files(char *frag_path, File *common_shader_code,
                       File *fragment_shaders) {
  int i;

  for (i = 0; i < FRAG_COUNT + 1; i++) {
    if (i == 0) {
      (*common_shader_code) = read_fragment_shader_file(frag_path, i);
    } else {
      fragment_shaders[i - 1] = read_fragment_shader_file(frag_path, i);

      file_prepend(&fragment_shaders[i - 1], *common_shader_code);
    }
  }
}

static void free_files(File *common_shader_code, File *fragment_shaders) {
  int i;

  for (i = 0; i < FRAG_COUNT; i++) {
    file_free(&fragment_shaders[i]);
  }

  file_free(common_shader_code);
}

void forge_run(Parameters params) {
  File fragment_shaders[FRAG_COUNT];
  File common_shader_code;
  ShaderProgram program;
  Window *window;
  Timer timer;
  Context context;

  init_files(params.frag_path, &common_shader_code, fragment_shaders);

  window = window_init(PACKAGE " " VERSION, params.screen, error_callback,
                       key_callback);

  context = window_get_context(window);

  program = shaders_init(fragment_shaders, context);

  if (program.error) {
    window_close(window, true);
    exit(EXIT_FAILURE);
  }

  timer = timer_init(30);

  while (!window_should_close(window)) {
    loop(window, program, params.hot_reload, &common_shader_code,
         fragment_shaders, &timer);
  }

  window_close(window, true);

  free_files(&common_shader_code, fragment_shaders);
}