// SRC A
// ---------
// IN: 0 / 1
// OUT: 3

layout(location = 3) out vec3 fragColor;

void main() {
    vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);
    vec3 color = vec3(vUV, sin(iTime * 0.5) * 0.5 + 0.5);
    color *= 1 - step(cos(iTime) * 0.1 + 0.4, length(uv1));
    fragColor = color + texture(frame0, vUV - 0.04).xyz * 0.5;
}