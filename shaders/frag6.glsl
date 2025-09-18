// MFX
// ------------
// IN: 8 (A+B)
// IN: 0 (OUT)
// OUT: 0 (OUT)

in vec2 vUV;
layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = fx_stage(vUV, tex7, tex0);
}