#version 460

uniform float iTime;
uniform vec2 iResolution;

uniform sampler2D frame7;
uniform sampler2D frame0;

in vec2 vUV;
layout(location = 0) out vec3 fragColor;

// MFX
// ---------
// IN: 7 / 0
// OUT: 0

void main() {
    fragColor = texture(frame7, vUV).xyz;
}