// FX B
// -------------
// IN: 6 (SRC B)
// IN: 8 (FX B)
// OUT: 8 (A+B)

in vec2 vUV;
out vec4 fragColor;

void main() {
    fragColor = fx_stage(vUV, tex6, tex8, seed6, src2_2[0], src2_2[1].xy, src2_2[2], src2_2[3].xy, src2_2[4], src2_2[5].xy, src2_2[6].xy);
}