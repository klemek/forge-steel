// SRC A
// -----------
// OUT: 5 (FX A)

in vec2 vUV;
out vec4 fragColor;

uniform vec3 iMidi1_1[6];

void main() {
    fragColor = src_stage(vUV, iSeed3, iMidi1_1[0], iMidi1_1[1].xy, iMidi1_1[2], iMidi1_1[3].xy, iMidi1_1[4], iMidi1_1[5].xy);
}