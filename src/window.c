#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include <linmath.h>

#include "config.h"
#include "types.h"

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#endif

// TODO split into smaller functions
// TODO custom struct to remove glfw in signature
void *init_window(Window **window, Parameters params,
                  void (*error_callback)(int, const char *),
                  void (*key_callback)(Window *, int, int, int, int)) {
  // set errors handler
  glfwSetErrorCallback(error_callback);

  // print current GLFW version
  fprintf(stdout, "[GLFW] %s\n", glfwGetVersionString());

  // init GLFW
  if (!glfwInit()) {
    fprintf(stderr, "[GLFW] Initialization failed\n");
    exit(EXIT_FAILURE);
  }

  // add context to window before creation
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DECORATED, 0);

  // detect monitors
  int count;
  GLFWmonitor **monitors = glfwGetMonitors(&count);

  // check selected monitor availability
  if (params.screen >= count) {
    fprintf(stderr, "Screen %d is out of range [0-%d]\n", params.screen,
            count - 1);
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // create fullscreen window in selected monitor
  (*window) = glfwCreateWindow(1, 1, PACKAGE " " VERSION,
                               monitors[params.screen], NULL);

  // handle window creation fail
  if (!(*window)) {
    fprintf(stderr, "[GLFW] Window or context creation failed\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // use current window
  glfwMakeContextCurrent((*window));
  // link GLAD and GLFW window
  gladLoadGL(glfwGetProcAddress);
  // set keyboard handler
  glfwSetKeyCallback((*window), key_callback);
  // hide cursor
  glfwSetInputMode((*window), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  // vsync
  glfwSwapInterval(1);

  return window;
}