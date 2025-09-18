// SRC A
// -----------
// IN: 0 (OUT)
// IN: 1 (IN A)
// OUT: 2 (FX A)

in vec2 vUV;
layout(location = 2) out vec4 fragColor;

void main() {
    fragColor = src_stage(vUV);
}