#include <GLFW/glfw3.h>
#include <log.h>
#include <stdbool.h>
#include <stdlib.h>

#include "types.h"
#include "window.h"

static void init_glfw(void (*error_callback)(int, const char *)) {
  log_info("[GLFW] Initializing...");

  // set errors handler
  glfwSetErrorCallback(error_callback);

  // print current GLFW version
  log_info("[GLFW] %s", glfwGetVersionString());

  // init GLFW
  if (!glfwInit()) {
    log_error("[GLFW] Initialization failed");
    exit(EXIT_FAILURE);
  }

  log_info("[GLFS] Initialized...");
}

static GLFWmonitor *get_monitor(unsigned char monitor_index) {
  int count;
  GLFWmonitor **monitors;

  // detect monitors
  monitors = glfwGetMonitors(&count);

  // check selected monitor availability
  if (monitor_index >= count) {
    log_error("[GLFW] Screen %d is out of range [0-%d]", monitor_index,
              count - 1);
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  return monitors[monitor_index];
}

static GLFWwindow *
create_window(GLFWmonitor *monitor, char *title, Window *shared_context,
              void (*key_callback)(Window *, int, int, int, int)) {
  GLFWwindow *window;

  log_info("[GLFW] Creating window...");

  // Window related hints
  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
  glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_FALSE);
  glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);

  // Context related hints
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // create fullscreen window in selected monitor
  window = glfwCreateWindow(1, 1, title, monitor, shared_context);

  // handle window creation fail
  if (window == NULL) {
    log_error("[GLFW] Window or context creation failed");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // set keyboard handler
  glfwSetKeyCallback(window, key_callback);
  // hide cursor
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

  log_info("[GLFW] Window created");

  return window;
}

static void use_window(GLFWwindow *window) { glfwMakeContextCurrent(window); }

void window_startup(void (*error_callback)(int, const char *)) {
  init_glfw(error_callback);
}

void window_terminate() {
  log_info("[GLFW] Terminating library...");
  glfwTerminate();
}

Window *window_init(char *title, unsigned char monitor_index, bool windowed,
                    Window *shared_context,
                    void (*key_callback)(Window *, int, int, int, int)) {
  GLFWwindow *window;
  GLFWmonitor *monitor;

  monitor = windowed ? NULL : get_monitor(monitor_index);

  window = create_window(monitor, title, shared_context, key_callback);

  use_window(window);

  return window;
}

void window_update_title(Window *window, char *title) {
  glfwSetWindowTitle(window, title);
}

void window_refresh(Window *window) {
  // swap front and back buffers
  glfwSwapBuffers(window);
}

void window_events() { glfwPollEvents(); }

double window_get_time() { return glfwGetTime(); }

void window_use(Window *window, SharedContext *context) {
  glfwMakeContextCurrent(window);
  glfwGetFramebufferSize(window, &context->width, &context->height);
  context->internal_width = (int)(context->internal_height *
                                  (float)context->width / (context->height));
}

void window_close(Window *window) {
  log_info("[GLFW] Closing window...");
  glfwSetWindowShouldClose(window, GLFW_TRUE);
}

bool window_should_close(Window *window) {
  return glfwWindowShouldClose(window) == GLFW_TRUE;
}

bool window_escape_key(int key, int action) {
  return key == GLFW_KEY_ESCAPE && action == GLFW_PRESS;
}

bool window_char_key(int key, int action, const int char_code) {
  return key == char_code && action == GLFW_PRESS;
}