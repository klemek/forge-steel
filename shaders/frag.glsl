#version 330

uniform float iTime;
uniform vec2 iResolution;
in vec2 vUV;
out vec4 fragColor;

void main() {
    vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);
    vec3 color = vec3(vUV, sin(iTime * 0.5) * 0.5 + 0.5);
    color *= 1 - step(cos(iTime) * 0.1 + 0.5,length(uv1));
    fragColor = vec4(color, 1.0);
}