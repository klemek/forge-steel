// A+B
// ------------
// IN: 5 (FX A)
// IN: 6 (FX B)
// OUT: 7 (MFX)

in vec2 vUV;
layout(location = 7) out vec3 fragColor;

void main() {
    fragColor = mix_stage(vUV, frame5, frame6);
}