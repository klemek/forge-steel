// SRC B
// ---------
// IN: 0 / 2
// OUT: 4

layout(location = 4) out vec3 fragColor;

void main() {
    fragColor = vec3(vUV, 0.0) * step(0.3, vUV.x) * step(-0.4, -vUV.x);
}