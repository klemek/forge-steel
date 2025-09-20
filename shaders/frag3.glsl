// FX A
// -------------
// IN: 2 (SRC A)
// IN: 3 (FX A)
// OUT: 3 (A+B)

in vec2 vUV;
out vec4 fragColor;

void main() {
    fragColor = fx_stage(vUV, tex2, tex3, seed3);
}