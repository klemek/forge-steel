// SRC B
// -----------
// IN: 0 (MFX)
// IN: 2 (IN B)
// OUT: 4 (FX B)

in vec2 vUV;
layout(location = 4) out vec3 fragColor;

void main() {
    fragColor = src_stage(vUV);
}