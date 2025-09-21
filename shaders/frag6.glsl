// FX B
// -------------
// IN: 6 (SRC B)
// IN: 8 (FX B)
// OUT: 8 (A+B)

in vec2 vUV;
out vec4 fragColor;

void main() {
    fragColor = fx_stage(vUV, tex6, tex8, seed6);
}