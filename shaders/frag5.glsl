// FX A
// -------------
// IN: 5 (SRC A)
// IN: 7 (FX A)
// OUT: 7 (A+B)

in vec2 vUV;
out vec4 fragColor;

void main() {
    fragColor = fx_stage(vUV, tex5, tex7, seed5);
}