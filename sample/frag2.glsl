#version 460

in vec2 vUV;
out vec4 fragColor;

uniform sampler2D iTex0;

void main() {
    fragColor = texture(iTex0, vUV);
}