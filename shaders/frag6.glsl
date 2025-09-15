// MFX
// ---------
// IN: 7 / 0
// OUT: 0

layout(location = 0) out vec3 fragColor;

void main() {
    fragColor = texture(frame7, vUV).xyz;
}