// SRC B
// -----------
// IN: 0 (OUT)
// IN: 4 (IN B)
// OUT: 5 (FX B)

in vec2 vUV;
layout(location = 5) out vec3 fragColor;

void main() {
    fragColor = src_stage(vUV);
}