#include <GLFW/glfw3.h>
#include <linmath.h>
#include <stdbool.h>
#include <stdlib.h>

#include "logs.h"
#include "types.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

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

  // Context related hints
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  log_success("[GLFS] Initialized...");
}

static GLFWmonitor *get_monitor(unsigned char monitor_index) {
  // detect monitors
  int count;
  GLFWmonitor **monitors = glfwGetMonitors(&count);

  // check selected monitor availability
  if (monitor_index >= count) {
    log_error("[GLFW] Screen %d is out of range [0-%d]", monitor_index,
              count - 1);
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  return monitors[monitor_index];
}

static GLFWwindow *create_window(GLFWmonitor *monitor, char *title,
                                 void (*key_callback)(Window *, int, int, int,
                                                      int)) {

  log_info("[GLFW] Creating window...");

  // Window related hints
  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
  glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_FALSE);
  glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);

  // create fullscreen window in selected monitor
  GLFWwindow *window = glfwCreateWindow(1, 1, title, monitor, NULL);

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

  log_success("[GLFW] Window created");

  return window;
}

static void use_window(GLFWwindow *window) {
  // use current window
  glfwMakeContextCurrent(window);
  // link GLAD and GLFW window
  gladLoadGL(glfwGetProcAddress);
  // vsync
  glfwSwapInterval(1);
}

Window *window_init(char *title, unsigned char monitor_index,
                    void (*error_callback)(int, const char *),
                    void (*key_callback)(Window *, int, int, int, int)) {
  GLFWwindow *window;
  GLFWmonitor *monitor;

  init_glfw(error_callback);

  monitor = get_monitor(monitor_index);

  window = create_window(monitor, title, key_callback);

  use_window(window);

  return window;
}

void window_update_title(Window *window, char *title) {
  glfwSetWindowTitle(window, title);
}

void window_refresh(Window *window) {
  // swap front and back buffers
  glfwSwapBuffers(window);
  // listen to mouse and keyboard events
  glfwPollEvents();
}

Context window_get_context(Window *window) {
  Context context;

  glfwGetFramebufferSize(window, &context.width, &context.height);

  context.time = glfwGetTime();

  return context;
}

void window_close(Window *window, bool hard) {
  if (hard) {
    log_info("[GLFW] Terminating library...");
    glfwTerminate();
  } else {
    log_info("[GLFW] Closing window...");
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

bool window_should_close(Window *window) {
  return glfwWindowShouldClose(window) == GLFW_TRUE;
}

bool window_escape_key(int key, int action) {
  return key == GLFW_KEY_ESCAPE && action == GLFW_PRESS;
}