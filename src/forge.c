#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "constants.h"
#include "file.h"
#include "types.h"

#include <linmath.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void error_callback(int error, const char *description) {
  fprintf(stderr, "Error %d: %s\n", error, description);
  glfwTerminate();
  exit(EXIT_FAILURE);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  // close window on escape key
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

// TODO extract to window file
// TODO split into smaller functions
void *init_window(GLFWwindow **window, Parameters params) {
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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

// TODO extract to "shaders" file
// TODO split into smaller functions
ShaderProgram init_program(File fragment_shader) {
  ShaderProgram program = {};
  GLint status_params;

  glGenBuffers(1, &program.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, program.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  program.vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(program.vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShader(program.vertex_shader);

  glGetShaderiv(program.vertex_shader, GL_COMPILE_STATUS, &status_params);
  if (status_params == GL_FALSE) {
    program.error = true;
    // TODO use glGetShaderInfoLog( 	GLuint shader, GLsizei
    // maxLength, GLsizei *length, GLchar *infoLog);
  }

  program.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(program.fragment_shader, 1,
                 (const GLchar *const *)&fragment_shader.content, NULL);
  glCompileShader(program.fragment_shader);

  glGetShaderiv(program.fragment_shader, GL_COMPILE_STATUS, &status_params);
  if (status_params == GL_FALSE) {
    program.error = true;
  }

  if (program.error) {
    return program;
  }

  program.program = glCreateProgram();
  glAttachShader(program.program, program.vertex_shader);
  glAttachShader(program.program, program.fragment_shader);
  glLinkProgram(program.program);

  program.mvp_location = glGetUniformLocation(program.program, "mvp");
  program.itime_location = glGetUniformLocation(program.program, "iTime");
  program.ires_location = glGetUniformLocation(program.program, "iResolution");
  program.vpos_location = glGetAttribLocation(program.program, "vPos");

  glGenVertexArrays(1, &program.vertex_array);
  glBindVertexArray(program.vertex_array);
  glEnableVertexAttribArray(program.vpos_location);
  glVertexAttribPointer(program.vpos_location, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void *)offsetof(Vertex, pos));

  return program;
}

// TODO extract to "shaders" file
void update_program(ShaderProgram program, File fragment_shader) {
  GLint status_params;
  glShaderSource(program.fragment_shader, 1,
                 (const GLchar *const *)&fragment_shader.content, NULL);
  glCompileShader(program.fragment_shader);

  glGetShaderiv(program.fragment_shader, GL_COMPILE_STATUS, &status_params);
  if (status_params == GL_FALSE) {
    fprintf(stderr, "Failed to compile shaders\n"); // TODO add info
    return;
  }
  glLinkProgram(program.program);
}

void loop(GLFWwindow *window, ShaderProgram program) {
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
  GLFWwindow *window;

  File fragment_shader = read_file("shaders/tmp.glsl");

  if (fragment_shader.error) {
    fprintf(stderr, "Cannot read file\n");
    exit(EXIT_FAILURE);
  }

  init_window(&window, params);

  ShaderProgram program = init_program(fragment_shader);

  if (program.error) {
    fprintf(stderr, "Failed to compile shaders\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  while (!glfwWindowShouldClose(window)) {
    if (should_update_file(&fragment_shader)) {
      update_file(&fragment_shader);
      update_program(program, fragment_shader);
    }
    loop(window, program);
  }

  glfwTerminate();

  free_file(&fragment_shader);
}