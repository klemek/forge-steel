// VIDEO 2
// -----------
// IN: 2 (RAW IN B)
// OUT: 4 (IN B)

in vec2 vUV;
out vec4 fragColor;

void main() {
    fragColor = yuyvTex(tex2, vUV, 320);  // TODO uniform
}