#version 460

// MONITOR
// ---

in vec2 vUV;
out vec4 fragColor;

#include inc_debug.glsl

uniform sampler2D iTex0;
uniform sampler2D iTex1;
uniform sampler2D iTex2;
uniform sampler2D iTex3;
uniform sampler2D iTex4;
uniform sampler2D iTex5;
uniform sampler2D iTex6;
uniform sampler2D iTex7;
uniform sampler2D iTex8;
uniform sampler2D iTex9;
uniform int iFPS;
uniform int iInputFPS1;
uniform int iInputFPS2;

float s(vec2 uv, float x0, float y0) {
    return step(x0, uv.x) * step(-x0 - 1, -uv.x) * step(y0, uv.y) *
step(-y0 - 1, -uv.y);
}

const int texts[12][5] = {
    {0x49, 0x4E, 0x20, 0x41, 0x00}, // IN A
    {0x49, 0x4E, 0x20, 0x42, 0x00}, // IN B
    {0x53, 0x52, 0x43, 0x20, 0x41}, // SRC A
    {0x53, 0x52, 0x43, 0x20, 0x42}, // SRC B
    {0x46, 0x58, 0x20, 0x41, 0x00}, // FX A
    {0x46, 0x58, 0x20, 0x42, 0x00}, // FX B
    {0x41, 0x2B, 0x42, 0x00, 0x00}, // A+B
    {0x4D, 0x46, 0x58, 0x00, 0x00}, // MFX
    {0x46, 0x50, 0x53, 0x00, 0x00}, // FPS
    {0x4F, 0x46, 0x46, 0x00, 0x00}, // OFF
    {0x44, 0x45, 0x4D, 0x4F, 0x00}, // DEMO
    {0x4C, 0x49, 0x56, 0x45, 0x00}, // LIVE
};

void main() {
    vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    vec2 uv2 = uv0 * 3;
    vec2 uv3 = uv1 * 60;

    vec4 c = vec4(0);
    
    c += s(uv2,1,2) * texture(iTex5, uv2);
    c += s(uv2,2,2) * texture(iTex7, uv2);

    c += s(uv2,1,0) * texture(iTex6, uv2);
    c += s(uv2,2,0) * texture(iTex8, uv2);

    c += s(uv2,0,1) * debug(mod(uv2, 1));
    c += s(uv2,1,1) * texture(iTex9, uv2);
    c += s(uv2,2,1) * texture(iTex0, uv2);

    float sel = 0;

    sel += iSelected == 3 ? h_rect(uv2, vec2(1.5, 2.5), vec2(0.5), 0.01) : 0;
    sel += iSelected == 4 ? h_rect(uv2, vec2(1.5, 0.5), vec2(0.5), 0.01) : 0;
    sel += iSelected == 5 ? h_rect(uv2, vec2(2.5, 2.5), vec2(0.5), 0.01) : 0;
    sel += iSelected == 6 ? h_rect(uv2, vec2(2.5, 0.5), vec2(0.5), 0.01) : 0;
    sel += iSelected == 8 ? h_rect(uv2, vec2(2.5, 1.5), vec2(0.5), 0.01) : 0;

    c = mix(c, 1 - c, sel);

    float f = 0;
    float t = 0;

    f += rect(uv3, vec2(-35, 28.5), vec2(2.1, 0.7));
    t += write_5(uv3, vec2(-37,28), texts[0]);
    
    if (iInputResolution1.x > 0) {
        c += s(uv2,0,2) * texture(iTex3, uv2);
        f += rect(uv3, vec2(-35, 26.75), vec2(2.8, 0.7));
        t += write_int(uv3, vec2(-37.6,26.1), iInputFPS1, 2);
        t += write_5(uv3, vec2(-35.1,26.1), texts[8]);
    } else {
        f += rect(uv3, vec2(-35, 26.75), vec2(1.6, 0.7));
        t += write_5(uv3, vec2(-36.5,26.1), texts[9]);
    }
   
    f += rect(uv3, vec2(-35, -11.5), vec2(2.1, 0.7));
    t += write_5(uv3, vec2(-37,-12), texts[1]);

    if (iInputResolution2.x > 0) {
        c += s(uv2,0,0) * texture(iTex4, uv2);
        f += rect(uv3, vec2(-35, -13.25), vec2(2.8, 0.7));
        t += write_int(uv3, vec2(-37.6,-13.9), iInputFPS2, 2);
        t += write_5(uv3, vec2(-35.1,-13.9), texts[8]);
    } else {
        f += rect(uv3, vec2(-35, -13.25), vec2(1.6, 0.7));
        t += write_5(uv3, vec2(-36.5,-13.9), texts[9]);
    }

    f += rect(uv3, vec2(-35, 8.5), vec2(2.1, 0.7));

    if (iDemo > 0) {
        t += write_5(uv3, vec2(-37,8), texts[10]);
    } else {
        t += write_5(uv3, vec2(-37,8), texts[11]);
    }

    f += rect(uv3, vec2(0, 28.5), vec2(2.6, 0.7));
    f += iActive1 == 1 ? h_rect(uv3, vec2(0, 28.5), vec2(3, 1), 0.2) : 0;
    t += write_5(uv3, vec2(-2.5,28), texts[2]);

    f += rect(uv3, vec2(0, -11.5), vec2(2.6, 0.7));
    f += iActive1 == 2 ? h_rect(uv3, vec2(0, -11.5), vec2(3, 1), 0.2) : 0;
    t += write_5(uv3, vec2(-2.5,-12), texts[3]);

    f += rect(uv3, vec2(35, 28.5), vec2(2.1, 0.7));
    f += iActive2 == 1 ? h_rect(uv3, vec2(35, 28.5), vec2(2.5, 1), 0.2) : 0;
    t += write_5(uv3, vec2(33,28), texts[4]);

    f += rect(uv3, vec2(35, -11.5), vec2(2.1, 0.7));
    f += iActive2 == 2 ? h_rect(uv3, vec2(35, -11.5), vec2(2.5, 1), 0.2) : 0;
    t += write_5(uv3, vec2(33,-12), texts[5]);

    f += rect(uv3, vec2(0, 8.5), vec2(1.6, 0.7));
    t += write_5(uv3, vec2(-1.5,8), texts[6]);

    f += rect(uv3, vec2(35, 8.5), vec2(1.6, 0.7));
    f += iActive2 == 3 ? h_rect(uv3, vec2(35, 8.5), vec2(2, 1), 0.2) : 0;
    t += write_5(uv3, vec2(33.5,8), texts[7]);

    f += rect(uv3, vec2(35, 6.75), vec2(2.8, 0.7));
    t += write_int(uv3, vec2(32.4,6.1), iFPS, 2);
    t += write_5(uv3, vec2(34.9,6.1), texts[8]);

    fragColor = mix(c, vec4(min(1, f) - t), min(1, f));
}