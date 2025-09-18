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

static char *output_shader_text = "#version 460\n"
                                  "in vec2 vUV;\n"
                                  "out vec4 fragColor;\n"
                                  "uniform sampler2D tex0;\n"
                                  "void main()\n"
                                  "{\n"
                                  "    fragColor = texture(tex0, vUV);\n"
                                  "}";

static char *monitor_shader_text =
    "#version 460\n"
    "uniform sampler2D tex0;\n"
    "uniform sampler2D tex1;\n"
    "uniform sampler2D tex2;\n"
    "uniform sampler2D tex3;\n"
    "uniform sampler2D tex4;\n"
    "uniform sampler2D tex5;\n"
    "uniform sampler2D tex6;\n"
    "uniform sampler2D tex7;\n"
    "uniform sampler2D tex8;\n"
    "in vec2 vUV;\n"
    "out vec4 fragColor;\n"
    "float s(vec2 uv, float x0, float y0) {\n"
    "    return step(x0, uv.x) * step(-x0 - 1, -uv.x) * step(y0, uv.y) * "
    "step(-y0 - 1, -uv.y);\n"
    "}\n"
    "void main() {\n"
    "    vec2 uv = vUV * 3;\n"
    "    fragColor = vec4(0);\n"
    "    fragColor += s(uv,0,2) * texture(tex1, uv);\n"
    "    fragColor += s(uv,1,2) * texture(tex2, uv);\n"
    "    fragColor += s(uv,2,2) * texture(tex3, uv);\n"
    "    fragColor += s(uv,0,1) * texture(tex4, uv);\n"
    "    fragColor += s(uv,1,1) * texture(tex5, uv);\n"
    "    fragColor += s(uv,2,1) * texture(tex6, uv);\n"
    "    fragColor += s(uv,0,0) * texture(tex7, uv);\n"
    "    fragColor += s(uv,1,0) * texture(tex8, uv);\n"
    "    fragColor += s(uv,2,0) * texture(tex0, uv);\n"
    "}";

static const Vertex vertices[6] = {{{0.0f, 0.0f}}, {{0.0f, 1.0f}},
                                   {{1.0f, 1.0f}}, {{0.0f, 0.0f}},
                                   {{1.0f, 1.0f}}, {{1.0f, 0.0f}}};

#endif /* CONSTANTS_H */