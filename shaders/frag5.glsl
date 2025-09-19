// A+B
// ------------
// IN: 3 (FX A)
// IN: 6 (FX B)
// OUT: 7 (MFX)

in vec2 vUV;
out vec4 fragColor;

const float seed = 334;

void main() {
    fragColor = mix_stage(vUV, tex3, tex6, seed);
}