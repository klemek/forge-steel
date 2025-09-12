#include "types.h"

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

static const Vertex vertices[6] = {{{0.0f, 0.0f}}, {{0.0f, 1.0f}},
                                   {{1.0f, 1.0f}}, {{0.0f, 0.0f}},
                                   {{1.0f, 1.0f}}, {{1.0f, 0.0f}}};