#version 460

uniform float iTime;
uniform vec2 iResolution;

uniform sampler2D frame5;
uniform sampler2D frame6;

in vec2 vUV;
layout(location = 7) out vec3 fragColor;

// A+B
// ---------
// IN: 5 / 6
// OUT: 7

void main() {
    fragColor = texture(frame5, vUV).xyz + texture(frame6, vUV).xyz;
}