// MONITOR
// ---

in vec2 vUV;
out vec4 fragColor;

float s(vec2 uv, float x0, float y0) {
    return step(x0, uv.x) * step(-x0 - 1, -uv.x) * step(y0, uv.y) *
step(-y0 - 1, -uv.y);
}

const int texts[8][5] = {
    {0x49, 0x4E, 0x20, 0x41, 0x00}, // IN A
    {0x49, 0x4E, 0x20, 0x42, 0x00}, // IN B
    {0x53, 0x52, 0x43, 0x20, 0x41}, // SRC A
    {0x53, 0x52, 0x43, 0x20, 0x42}, // SRC B
    {0x46, 0x58, 0x20, 0x41, 0x00}, // FX A
    {0x46, 0x58, 0x20, 0x42, 0x00}, // FX B
    {0x41, 0x2B, 0x42, 0x00, 0x00}, // A+B
    {0x4D, 0x46, 0x58, 0x00, 0x00}, // MFX
};

void main() {
    vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    vec2 uv2 = uv0 * 3;
    vec2 uv3 = uv1 * 60;

    vec4 c = vec4(0);
    
    c += s(uv2,0,2) * texture(tex1, uv2);
    c += s(uv2,1,2) * texture(tex2, uv2);
    c += s(uv2,2,2) * texture(tex3, uv2);
    c += s(uv2,0,1) * src_16(mod(uv2, 1), 0);
    c += s(uv2,1,1) * texture(tex7, uv2);
    c += s(uv2,2,1) * texture(tex0, uv2);
    c += s(uv2,0,0) * texture(tex4, uv2);
    c += s(uv2,1,0) * texture(tex5, uv2);
    c += s(uv2,2,0) * texture(tex6, uv2);

    float t = 0;

    t += write_5(uv3, vec2(-53,28), texts[0]);
    t += write_5(uv3, vec2(-53,-12), texts[1]);
    t += write_5(uv3, vec2(-17,28), texts[2]);
    t += write_5(uv3, vec2(-17,-12), texts[3]);
    t += write_5(uv3, vec2(19,28), texts[4]);
    t += write_5(uv3, vec2(19,-12), texts[5]);
    t += write_5(uv3, vec2(-17,8), texts[6]);
    t += write_5(uv3, vec2(19,8), texts[7]);

    fragColor = mix(c, 1 - c, t);
}