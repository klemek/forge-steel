// FX B
// -------------
// IN: 4 (SRC B)
// IN: 6 (FX B)
// OUT: 6 (A+B)

in vec2 vUV;
layout(location = 6) out vec3 fragColor;

void main() {
    fragColor = fx_stage(vUV, frame4, frame6);
}