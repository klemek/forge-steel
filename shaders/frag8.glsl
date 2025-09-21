// MFX
// ------------
// IN: 9 (A+B)
// IN: 0 (OUT)
// OUT: 0 (OUT)

in vec2 vUV;
out vec4 fragColor;

void main() {
    fragColor = fx_stage(vUV, tex9, tex0, seed8);
}