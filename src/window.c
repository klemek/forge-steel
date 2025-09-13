#include <GLFW/glfw3.h>
#include <linmath.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "types.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

void init_glfw(void (*error_callback)(int, const char *)) {
  // set errors handler
  glfwSetErrorCallback(error_callback);

  // print current GLFW version
  fprintf(stdout, "[GLFW] %s\n", glfwGetVersionString());

  // init GLFW
  if (!glfwInit()) {
    fprintf(stderr, "[GLFW] Initialization failed\n");
    exit(EXIT_FAILURE);
  }

  // Context related hints
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWmonitor *get_monitor(unsigned char screen_index) {
  // detect monitors
  int count;
  GLFWmonitor **monitors = glfwGetMonitors(&count);

  // check selected monitor availability
  if (screen_index >= count) {
    fprintf(stderr, "Screen %d is out of range [0-%d]\n", screen_index,
            count - 1);
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  return monitors[screen_index];
}

GLFWwindow *create_window(GLFWmonitor *monitor,
                          void (*key_callback)(Window *, int, int, int, int)) {
  // Window related hints
  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

  // create fullscreen window in selected monitor
  GLFWwindow *window =
      glfwCreateWindow(1, 1, PACKAGE " " VERSION, monitor, NULL);

  // handle window creation fail
  if (!window) {
    fprintf(stderr, "[GLFW] Window or context creation failed\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // set keyboard handler
  glfwSetKeyCallback(window, key_callback);
  // hide cursor
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

  return window;
}

void use_window(GLFWwindow *window) {
  // use current window
  glfwMakeContextCurrent(window);
  // link GLAD and GLFW window
  gladLoadGL(glfwGetProcAddress);
  // vsync
  glfwSwapInterval(1);
}

Window *init_window(Parameters params,
                    void (*error_callback)(int, const char *),
                    void (*key_callback)(Window *, int, int, int, int)) {
  GLFWwindow *window;
  GLFWmonitor *monitor;

  init_glfw(error_callback);

  monitor = get_monitor(params.screen);

  window = create_window(monitor, key_callback);

  use_window(window);

  return window;
}