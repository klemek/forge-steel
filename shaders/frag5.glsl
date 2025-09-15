// A+B
// ---------
// IN: 5 / 6
// OUT: 7

in vec2 vUV;
layout(location = 7) out vec3 fragColor;

void main() {
    fragColor = texture(frame5, vUV).xyz + texture(frame6, vUV).xyz;
}