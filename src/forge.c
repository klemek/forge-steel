#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>
#include <linmath.h>

#include "file.h"
#include "shaders.h"
#include "types.h"
#include "window.h"

void error_callback(int error, const char *description) {
  fprintf(stderr, "Error %d: %s\n", error, description);
  glfwTerminate();
  exit(EXIT_FAILURE);
}

static void key_callback(Window *window, int key,
                         __attribute__((unused)) int scancode, int action,
                         int mods) {
  // close window on escape key
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && mods == 0) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

void loop(Window *window, ShaderProgram program) {
  // TODO remove GLFW references
  // TODO split into smaller functions
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  vec2 resolution = {(float)width, (float)height};

  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT);

  mat4x4 m, p, mvp;
  mat4x4_identity(m);
  mat4x4_ortho(p, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
  mat4x4_mul(mvp, p, m);

  glUseProgram(program.program);
  glUniformMatrix4fv(program.mvp_location, 1, GL_FALSE, (const GLfloat *)&mvp);
  glUniform1f(program.itime_location, (const GLfloat)glfwGetTime());
  glUniform2fv(program.ires_location, 1, (const GLfloat *)&resolution);
  glBindVertexArray(program.vertex_array);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glfwSwapBuffers(window);
  glfwPollEvents();
}

void forge_run(Parameters params) {
  // TODO remove GLFW references
  Window *window;

  File fragment_shader = read_file(params.frag_path);

  if (fragment_shader.error) {
    fprintf(stderr, "Cannot read file\n");
    exit(EXIT_FAILURE);
  }

  init_window(&window, params, error_callback, key_callback);

  ShaderProgram program = init_program(fragment_shader);

  if (program.error) {
    fprintf(stderr, "Failed to compile shaders\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  while (!glfwWindowShouldClose(window)) {
    if (params.hot_reload && should_update_file(&fragment_shader)) {
      update_file(&fragment_shader);
      update_program(program, fragment_shader);
    }
    loop(window, program);
  }

  glfwTerminate();

  free_file(&fragment_shader);
}