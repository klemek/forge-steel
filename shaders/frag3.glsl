// SRC A
// -----------
// OUT: 5 (FX A)

in vec2 vUV;
out vec4 fragColor;

uniform vec3 src1_1[6];

void main() {
    fragColor = src_stage(vUV, seed3, src1_1[0], src1_1[1].xy, src1_1[2], src1_1[3].xy, src1_1[4], src1_1[5].xy);
}