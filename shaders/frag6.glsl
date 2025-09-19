// MFX
// ------------
// IN: 8 (A+B)
// IN: 0 (OUT)
// OUT: 0 (OUT)

in vec2 vUV;
out vec4 fragColor;

const float seed = 808;

void main() {
    fragColor = fx_stage(vUV, tex8, tex0, seed);
}