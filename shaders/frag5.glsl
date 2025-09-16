// A+B
// ------------
// IN: 5 (FX A)
// IN: 6 (FX B)
// OUT: 7 (MFX)

in vec2 vUV;
layout(location = 7) out vec3 fragColor;

void main() {
    // TODO subroutine
    fragColor = texture(frame5, vUV).xyz + texture(frame6, vUV).xyz;
}