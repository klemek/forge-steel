#include "types.h"

#ifndef CONSTANTS_H
#define CONSTANTS_H

static char *vertex_shader_text =
    "#version 460\n"
    "uniform mat4 mvp;\n"
    "in vec2 vPos;\n"
    "out vec2 vUV;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = mvp * vec4(vPos, 0.0, 1.0);\n"
    "    vUV = vPos;\n"
    "}\n";

static char *output_fragment_shader_text =
    "#version 460\n"
    "in vec2 vUV;\n"
    "out vec4 fragColor;\n"
    "uniform sampler2D frame0\n"
    "void main()\n"
    "{\n"
    "    fragColor = vec4(texture(frame0, vUV).xyz, 1.0);\n"
    "}\n";

static const Vertex vertices[6] = {{{0.0f, 0.0f}}, {{0.0f, 1.0f}},
                                   {{1.0f, 1.0f}}, {{0.0f, 0.0f}},
                                   {{1.0f, 1.0f}}, {{1.0f, 0.0f}}};

#endif