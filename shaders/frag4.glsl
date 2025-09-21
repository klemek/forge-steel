// SRC B
// -----------
// OUT: 6 (FX B)

in vec2 vUV;
out vec4 fragColor;

void main() {
    fragColor = src_stage(vUV, seed4);
}