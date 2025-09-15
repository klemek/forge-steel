// SRC B
// ---------
// IN: 0 / 2
// OUT: 4

in vec2 vUV;
layout(location = 4) out vec3 fragColor;

void main() {
    vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = uv0 * vec2(ratio, 1);
    vec2 uv2 = uv1 * 20;

    // uv2 = mod(uv2, 1);

    bool v = false;

    v = v || iFPS > 99 && char(uv2 - vec2(0.5, 0.5), 0x30 + (iFPS % 1000) / 100);
    v = v || iFPS > 9 && char(uv2 - vec2(1.5, 0.5), 0x30 + (iFPS % 100) / 10);
    v = v || char(uv2 - vec2(2.5, 0.5), 0x30 + (iFPS % 10));
    v = v || char(uv2 - vec2(4.0, 0.5), 0x66);
    v = v || char(uv2 - vec2(5.0, 0.5), 0x70);
    v = v || char(uv2 - vec2(6.0, 0.5), 0x73);

    fragColor = vec3(v ? 1 : 0);
}