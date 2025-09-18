// A+B
// ------------
// IN: 3 (FX A)
// IN: 6 (FX B)
// OUT: 7 (MFX)

in vec2 vUV;
layout(location = 7) out vec4 fragColor;

void main() {
    fragColor = mix_stage(vUV, tex3, tex6, sin(iTime * 0.25) * 0.3 + 0.3); // TODO tmp
}