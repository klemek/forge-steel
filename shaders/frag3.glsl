#version 460

uniform float iTime;
uniform vec2 iResolution;

uniform sampler2D frame3;
uniform sampler2D frame5;

in vec2 vUV;
layout(location = 5) out vec3 fragColor;

// FX A
// ---------
// IN: 3 / 5
// OUT: 5

void main() {
    fragColor = texture(frame3, vUV).xyz;
}