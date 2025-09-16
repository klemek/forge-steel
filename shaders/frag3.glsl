// FX A
// -------------
// IN: 3 (SRC A)
// IN: 5 (FX A)
// OUT: 5 (A+B)

in vec2 vUV;
layout(location = 5) out vec3 fragColor;

void main() {
    fragColor = fx_stage(vUV, frame3, frame5);
}