// SRC B
// -----------
// IN: 0 (OUT)
// IN: 4 (IN B)
// OUT: 5 (FX B)

in vec2 vUV;
out vec4 fragColor;

const float seed = 580;

void main() {
    fragColor = src_stage(vUV, seed);
}