#version 460

// FX A
// -------------
// IN: 7 (SRC A)
// IN: 9 (FX A)
// OUT: 9 (A+B)

in vec2 vUV;
out vec4 fragColor;

#include inc_fx.glsl

uniform sampler2D iTex7;
uniform sampler2D iTex9;
uniform int iSeed5;
uniform vec3 iMidi2_1[7];

void main() {
    fragColor = fx_stage(vUV, iTex7, iTex9, iSeed5, iMidi2_1[0], iMidi2_1[1].xy, iMidi2_1[2], iMidi2_1[3].xy, iMidi2_1[4], iMidi2_1[5].xy, iMidi2_1[6]);
}