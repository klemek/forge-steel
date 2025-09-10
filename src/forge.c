#include "config.h"
#include "types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GLFW/glfw3.h>

void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void forge_run(parameters params) {
  GLFWwindow *window;
  glfwSetErrorCallback(error_callback);

  fprintf(stdout, "[GLFW] %s\n", glfwGetVersionString());

  if (!glfwInit()) {
    fprintf(stderr, "[GLFW] Initialization failed\n");
    exit(1);
  }

  window = glfwCreateWindow(640, 480, PACKAGE " " VERSION, NULL, NULL);

  if (!window) {
    fprintf(stderr, "[GLFW] Window or context creation failed\n");
    glfwTerminate();
    exit(1);
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  glfwSwapInterval(1);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}