#version 460

// A+B
// ------------
// IN: 7 (FX A)
// IN: 8 (FX B)
// OUT: 9 (MFX)

in vec2 vUV;
out vec4 fragColor;

#include inc_magic.glsl
#include inc_functions.glsl

uniform int iDemo;
uniform sampler2D iTex7;
uniform sampler2D iTex8;
uniform int iSeed7;
uniform vec3 iMidi3_1[2];

void main() {
    float mix_value = magic(iMidi3_1[1].xy, vec3(1, 0, 0), iSeed7);
    bool mix_type = magic_trigger(vec3(iMidi3_1[0].x, 0, 0), iSeed7 + 10);

    vec4 color_a = texture(iTex7, vUV);
    vec4 color_b = texture(iTex8, vUV);

    float k = mean(color_a);

    mix_value = mix(mix_value, mix_value * 0.9 + 0.05, iDemo);

    fragColor = mix(color_b, color_a, mix_type ? step(mix_value, k) : mix_value);
}