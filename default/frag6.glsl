#version 460

// FX B
// -------------
// IN: 6 (SRC B)
// IN: 8 (FX B)
// OUT: 8 (A+B)

in vec2 vUV;
out vec4 fragColor;

#include inc_fx.glsl

uniform sampler2D iTex6;
uniform sampler2D iTex8;
uniform int iSeed6;
uniform vec3 iMidi2_2[7];

void main() {
    fragColor = fx_stage(vUV, iTex6, iTex8, iSeed6, iMidi2_2[0], iMidi2_2[1].xy, iMidi2_2[2], iMidi2_2[3].xy, iMidi2_2[4], iMidi2_2[5].xy, iMidi2_2[6]);
}