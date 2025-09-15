// FX B
// ---------
// IN: 4 / 6
// OUT: 6

in vec2 vUV;
layout(location = 6) out vec3 fragColor;

void main() {
    fragColor = texture(frame4, vUV).xyz;
}