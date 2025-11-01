// SRC B
// -----------
// OUT: 6 (FX B)

in vec2 vUV;
out vec4 fragColor;

uniform vec3 iMidi1_2[6];

void main() {
    fragColor = src_stage(vUV, iSeed4, iMidi1_2[0], iMidi1_2[1].xy, iMidi1_2[2], iMidi1_2[3].xy, iMidi1_2[4], iMidi1_2[5].xy);
}