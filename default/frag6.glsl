#include frag0.glsl

// FX B
// -------------
// IN: 6 (SRC B)
// IN: 8 (FX B)
// OUT: 8 (A+B)

in vec2 vUV;
out vec4 fragColor;

void main() {
    fragColor = fx_stage(vUV, iTex6, iTex8, iSeed6, iMidi2_2[0], iMidi2_2[1].xy, iMidi2_2[2], iMidi2_2[3].xy, iMidi2_2[4], iMidi2_2[5].xy, iMidi2_2[6]);
}