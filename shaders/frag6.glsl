// MFX
// ------------
// IN: 7 (A+B)
// IN: 0 (OUT)
// OUT: 0 (OUT)

in vec2 vUV;
layout(location = 0) out vec3 fragColor;

void main() {
    fragColor = fx_stage(vUV, frame7, frame0);
}