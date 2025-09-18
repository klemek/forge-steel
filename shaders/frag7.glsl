// OUT
// ---

in vec2 vUV;
out vec4 fragColor;

void main() {
    fragColor = texture(tex0, vUV);
}