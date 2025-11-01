// MFX
// ------------
// IN: 9 (A+B)
// IN: 0 (OUT)
// OUT: 0 (OUT)

in vec2 vUV;
out vec4 fragColor;

void main() {
    vec4 color = fx_stage(vUV, iTex9, iTex0, iSeed8, iMidi2_3[0], iMidi2_3[1].xy, iMidi2_3[2], iMidi2_3[3].xy, iMidi2_3[4], iMidi2_3[5].xy, iMidi2_3[6].xy);

    color = mix(color, vec4(0), iMidi3_1[0].y);

    color = mix(color, 1 - color, iMidi3_1[0].z);

    fragColor = color;
}