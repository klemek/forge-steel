#include frag0.glsl

// OUT
// ---

in vec2 vUV;
out vec4 fragColor;

void main() {
    fragColor = texture(iTex0, vUV);
}