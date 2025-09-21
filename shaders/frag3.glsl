// SRC A
// -----------
// OUT: 5 (FX A)

in vec2 vUV;
out vec4 fragColor;

void main() {
    fragColor = src_stage(vUV, seed3);
}