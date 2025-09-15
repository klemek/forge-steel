// FX A
// ---------
// IN: 3 / 5
// OUT: 5

in vec2 vUV;
layout(location = 5) out vec3 fragColor;

void main() {
    fragColor = gauss2(frame3, vUV, 0.001); //texture(frame3, vUV).xyz;
}