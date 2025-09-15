#version 460

uniform float iTime;
uniform vec2 iResolution;

uniform sampler2D frame0;
uniform sampler2D frame2;

in vec2 vUV;
layout(location = 4) out vec3 fragColor;

// SRC B
// ---------
// IN: 0 / 2
// OUT: 4

void main() {
    fragColor = vec3(vUV, 0.0) * step(0.3, vUV.x) * step(-0.4, -vUV.x);
}