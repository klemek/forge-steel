// FX A
// -------------
// IN: 5 (SRC A)
// IN: 7 (FX A)
// OUT: 7 (A+B)

in vec2 vUV;
out vec4 fragColor;

void main() {
    fragColor = fx_stage(vUV, tex5, tex7, seed5, src2_1[0], src2_1[1].xy, src2_1[2], src2_1[3].xy, src2_1[4], src2_1[5].xy, src2_1[6].xy);
}