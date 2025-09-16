// SRC A
// -----------
// IN: 0 (MFX)
// IN: 1 (IN A)
// OUT: 3 (FX A)

in vec2 vUV;
layout(location = 3) out vec3 fragColor;

void main() {
    fragColor = src_stage(vUV);
}