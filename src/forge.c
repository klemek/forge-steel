#include "config.h"
#include "types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "linmath.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

static const Vertex vertices[6] = {{{0.0f, 0.0f}}, {{0.0f, 1.0f}},
                                   {{1.0f, 1.0f}}, {{0.0f, 0.0f}},
                                   {{1.0f, 1.0f}}, {{1.0f, 0.0f}}};

static const char *vertex_shader_text =
    "#version 330\n"
    "uniform mat4 mvp;\n"
    "in vec2 vPos;\n"
    "out vec2 vUV;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = mvp * vec4(vPos, 0.0, 1.0);\n"
    "    vUV = vPos;\n"
    "}\n";

static const char *fragment_shader_text =
    "#version 330\n"
    "uniform float iTime;\n"
    "uniform vec2 iResolution;\n"
    "in vec2 vUV;\n"
    "out vec4 fragColor;\n"
    "void main()\n"
    "{\n"
    "    vec2 uv0 = vUV.st;\n"
    "    float ratio = iResolution.x / iResolution.y;\n"
    "    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);\n"
    "    vec3 color = vec3(vUV, 0.0);\n"
    "    color *= 1 - step(abs(sin(iTime) * 0.5),length(uv1));\n"
    "    fragColor = vec4(color, 1.0);\n"
    "}\n";

void error_callback(int error, const char *description) {
  fprintf(stderr, "Error %d: %s\n", error, description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

void *init_window(GLFWwindow **window) {
  glfwSetErrorCallback(error_callback);

  fprintf(stdout, "[GLFW] %s\n", glfwGetVersionString());

  if (!glfwInit()) {
    fprintf(stderr, "[GLFW] Initialization failed\n");
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  (*window) = glfwCreateWindow(640, 480, PACKAGE " " VERSION, NULL, NULL);

  if (!(*window)) {
    fprintf(stderr, "[GLFW] Window or context creation failed\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent((*window));
  gladLoadGL(glfwGetProcAddress);
  glfwSetKeyCallback((*window), key_callback);
  glfwSwapInterval(1);

  return window;
}

ShaderProgram init_program() {
  ShaderProgram program = {};

  glGenBuffers(1, &program.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, program.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  program.vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(program.vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShader(program.vertex_shader);

  program.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(program.fragment_shader, 1, &fragment_shader_text, NULL);
  glCompileShader(program.fragment_shader);

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

void loop(GLFWwindow *window, ShaderProgram program) {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  const float ratio = width / (float)height;
  vec2 resolution = {(float)width, (float)height};

  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT);

  mat4x4 m, p, mvp;
  mat4x4_identity(m);
  mat4x4_ortho(p, 0, ratio, 0.0f, 1.0f, 1.0f, 0.0f);
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

void forge_run(parameters params) {
  GLFWwindow *window;

  init_window(&window);

  ShaderProgram program = init_program();

  while (!glfwWindowShouldClose(window)) {
    loop(window, program);
  }

  glfwTerminate();
}