#version 460

// MFX
// ------------
// IN: 9 (A+B)
// IN: 0 (OUT)
// OUT: 0 (OUT)

in vec2 vUV;
out vec4 fragColor;

#include inc_fx.glsl

uniform sampler2D iTex9;
uniform sampler2D iTex0;
uniform int iSeed8;
uniform vec3 iMidi2_3[7];
uniform vec3 iMidi3_1[2];
uniform int iDemo;

void main() {
    vec4 color = fx_stage(vUV, iTex9, iTex0, iSeed8, iMidi2_3[0], iMidi2_3[1].xy, iMidi2_3[2], iMidi2_3[3].xy, iMidi2_3[4], iMidi2_3[5].xy, iMidi2_3[6]);

    if (iDemo < 1) {
        color = mix(color, vec4(0), iMidi3_1[0].y);
    }

    fragColor = color;
}