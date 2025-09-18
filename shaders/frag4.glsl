// FX B
// -------------
// IN: 5 (SRC B)
// IN: 6 (FX B)
// OUT: 6 (A+B)

in vec2 vUV;
layout(location = 6) out vec4 fragColor;

void main() {
    fragColor = fx_stage(vUV, tex5, tex6);
}