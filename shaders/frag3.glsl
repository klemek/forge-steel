#version 460

// FX A
// ---------
// IN: 3 / 5
// OUT: 5

layout(location = 5) out vec3 fragColor;

void main() {
    fragColor = texture(frame3, vUV).xyz;
}