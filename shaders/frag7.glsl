// A+B
// ------------
// IN: 7 (FX A)
// IN: 8 (FX B)
// OUT: 9 (MFX)

in vec2 vUV;
out vec4 fragColor;

void main() {
    fragColor = mix_stage(vUV, tex7, tex8, seed7);
}