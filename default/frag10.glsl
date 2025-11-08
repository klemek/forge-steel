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
uniform int iInputFPS1;
uniform int iInputFPS2;

float s(vec2 uv, float x0, float y0) {
    return step(x0, uv.x) * step(-x0 - 1, -uv.x) * step(y0, uv.y) *
step(-y0 - 1, -uv.y);
}

const int texts[10][5] = {
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

    c += s(uv2,1,1) * texture(iTex6, uv2);
    c += s(uv2,2,1) * texture(iTex8, uv2);

    c += s(uv2,0,0) * debug(mod(uv2, 1));
    c += s(uv2,1,0) * texture(iTex9, uv2);
    c += s(uv2,2,0) * texture(iTex0, uv2);

    float f = 0;
    float t = 0;

    f += rect(uv3, vec2(-51, 28.5), vec2(2.1, 0.7));
    t += write_5(uv3, vec2(-53,28), texts[0]);
    
    if (iInputResolution1.x > 0) {
        c += s(uv2,0,2) * texture(iTex3, uv2);
        f += rect(uv3, vec2(-50.4, 26.5), vec2(2.8, 0.7));
        t += write_int(uv3, vec2(-53,26), iInputFPS1, 2);
        t += write_5(uv3, vec2(-50.5,26), texts[8]);
    } else {
        f += rect(uv3, vec2(-51.5, 26.5), vec2(1.6, 0.7));
        t += write_5(uv3, vec2(-53,26), texts[9]);
    }
   
    f += rect(uv3, vec2(-51, 8.5), vec2(2.1, 0.7));
    t += write_5(uv3, vec2(-53,8), texts[1]);

    if (iInputResolution2.x > 0) {
        c += s(uv2,0,1) * texture(iTex4, uv2);
        f += rect(uv3, vec2(-50.4, 6.5), vec2(2.8, 0.7));
        t += write_int(uv3, vec2(-53,6), iInputFPS2, 2);
        t += write_5(uv3, vec2(-50.5,6), texts[8]);
    } else {
        f += rect(uv3, vec2(-51.5, 6.5), vec2(1.6, 0.7));
        t += write_5(uv3, vec2(-53,6), texts[9]);
    }
    f += rect(uv3, vec2(-14.5, 28.5), vec2(2.6, 0.7));
    t += write_5(uv3, vec2(-17,28), texts[2]);
    f += rect(uv3, vec2(-14.5, 8.5), vec2(2.6, 0.7));
    t += write_5(uv3, vec2(-17,8), texts[3]);
    f += rect(uv3, vec2(21, 28.5), vec2(2.1, 0.7));
    t += write_5(uv3, vec2(19,28), texts[4]);
    f += rect(uv3, vec2(21, 8.5), vec2(2.1, 0.7));
    t += write_5(uv3, vec2(19,8), texts[5]);
    f += rect(uv3, vec2(-15.5, -11.5), vec2(1.6, 0.7));
    t += write_5(uv3, vec2(-17,-12), texts[6]);
    f += rect(uv3, vec2(20.5, -11.5), vec2(1.6, 0.7));
    t += write_5(uv3, vec2(19,-12), texts[7]);

    fragColor = mix(c, vec4(f - t), f);
}