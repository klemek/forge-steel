// MFX
// ------------
// IN: 9 (A+B)
// IN: 0 (OUT)
// OUT: 0 (OUT)

in vec2 vUV;
out vec4 fragColor;

void main() {
    vec4 color = fx_stage(vUV, tex9, tex0, seed8, src2_3[0], src2_3[1].xy, src2_3[2], src2_3[3].xy, src2_3[4], src2_3[5].xy, src2_3[6].xy);

    color = mix(color, vec4(0), src3_1[0].y);

    color = mix(color, 1 - color, src3_1[0].z);

    fragColor = color;
}