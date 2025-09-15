#version 460

uniform float iTime;
uniform vec2 iResolution;

uniform sampler2D frame4;
uniform sampler2D frame6;

in vec2 vUV;
layout(location = 6) out vec3 fragColor;

// FX B
// ---------
// IN: 4 / 6
// OUT: 6

void main() {
    fragColor = texture(frame4, vUV).xyz;
}