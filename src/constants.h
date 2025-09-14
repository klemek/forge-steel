#include "types.h"

#ifndef CONSTANTS_H
#define CONSTANTS_H

static char *vertex_shader_text =
    "#version 460\n"
    "const mat4 mvp = "
    "{{2.,0.,0.,0.},{0.,2.,0.,0.},{0.,0.,2.,0.},{-1.,-1.,1.,1.}};\n"
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
    "uniform sampler2D frame0;\n"
    "void main()\n"
    "{\n"
    "    fragColor = texture(frame0, vUV);\n"
    "}\n";

static const Vertex vertices[6] = {{{0.0f, 0.0f}}, {{0.0f, 1.0f}},
                                   {{1.0f, 1.0f}}, {{0.0f, 0.0f}},
                                   {{1.0f, 1.0f}}, {{1.0f, 0.0f}}};

#endif