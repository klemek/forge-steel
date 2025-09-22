// VIDEO 1
// -----------
// IN: 1 (RAW IN A)
// OUT: 3 (IN A)

in vec2 vUV;
out vec4 fragColor;

void main() {
    fragColor = yuyvTex(tex1, vUV, 480); // TODO uniform
}