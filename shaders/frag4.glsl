// SRC B
// -----------
// OUT: 6 (FX B)

in vec2 vUV;
out vec4 fragColor;

uniform vec3 src1_2[6];

void main() {
    fragColor = src_stage(vUV, seed4, src1_2[0], src1_2[1].xy, src1_2[2], src1_2[3].xy, src1_2[4], src1_2[5].xy);
}