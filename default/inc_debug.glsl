#include inc_magic.glsl
#include inc_functions.glsl
#include inc_yuv.glsl
#include inc_cp437.glsl

#ifndef INC_DEBUG
#define INC_DEBUG

uniform int iFPS;

uniform vec2 iInputResolution1;
uniform vec2 iInputResolution2;
uniform int iInputFormat1;
uniform int iInputFormat2;

uniform int iDemo;
uniform int iAutoRand;
uniform int iAutoRandCycle;
uniform int iPage;
uniform int iSelected;

uniform int iSeed1;
uniform int iSeed2;
uniform int iSeed3;
uniform int iSeed4;
uniform int iSeed5;
uniform int iSeed6;
uniform int iSeed7;
uniform int iSeed8;

uniform int iState3;
uniform int iState4;
uniform int iState5;
uniform int iState6;
uniform int iState8;

uniform int iActive1;
uniform int iActive2;

uniform vec3 iMidi2_1[7];
uniform vec3 iMidi2_2[7];
uniform vec3 iMidi2_3[7];
uniform vec3 iMidi3_1[2];


vec4 debug(vec2 vUV)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // inputs

    int selected_srca = iState3 - 1;
    bool random_srca = iState3 == 15;
    if (random_srca) {
        selected_srca = int(randTime(iSeed3 + 100) * 14);
    }
    if (selected_srca == 5 && iDemo > 0) {
        selected_srca = 1;
        random_srca = true;
    }
    if (selected_srca == 10 && iDemo > 0) {
        selected_srca = 2;
        random_srca = true;
    }
    int selected_srcb = iState4 - 1;
    bool random_srcb = iState4 == 15;
    if (random_srcb) {
        selected_srcb = int(randTime(iSeed4 + 100) * 14);
    }
    if (selected_srcb == 5 && iDemo > 0) {
        selected_srcb = 1;
        random_srcb = true;
    }
    if (selected_srcb == 10 && iDemo > 0) {
        selected_srcb = 2;
        random_srcb = true;
    }
    int selected_fxa = iState5 - 1;
    bool random_fxa = iState5 == 15;
    if (random_fxa) {
        selected_fxa = int(randTime(iSeed5 + 100) * 14);
    }
    int selected_fxb = iState6 - 1;
    bool random_fxb = iState6 == 15;
    if (random_fxb) {
        selected_fxb = int(randTime(iSeed6 + 100) * 14);
    }
    int selected_mfx = iState8 - 1;
    bool random_mfx = iState8 == 15;
    if (random_mfx) {
        selected_mfx = int(randTime(iSeed8 + 100) * 14);
    }
    float fxa_value = magic(iMidi2_1[6].xy, vec3(1, 0, 0), iSeed5);
    bool fxa_invert = magic_trigger(vec3(iMidi2_1[6].z, 0, 0), iSeed5);
    float fxb_value = magic(iMidi2_2[6].xy, vec3(1, 0, 0), iSeed6);
    bool fxb_invert = magic_trigger(vec3(iMidi2_2[6].z, 0, 0), iSeed6);
    float mfx_value = magic(iMidi2_3[6].xy, vec3(1, 0, 0), iSeed8);
    bool mfx_invert = magic_trigger(vec3(iMidi2_3[6].z, 0, 0), iSeed8);
    float mix_value = magic(iMidi3_1[1].xy, vec3(1, 0, 0), iSeed7);
    mix_value = mix(mix_value, mix_value * 0.9 + 0.05, iDemo);
    bool mix_type = magic_trigger(vec3(iMidi3_1[0].x, 0, 0), iSeed7 + 10);

    // logic

    const int texts[10][5] = {
        {0x46, 0x50, 0x53, 0x00, 0x00}, // FPS
        {0x54, 0x45, 0x4D, 0x50, 0x4F}, // TEMPO
        {0x54, 0x49, 0x4D, 0x45, 0x00}, // TIME
        {0x44, 0x45, 0x4D, 0x4F, 0x00}, // DEMO
        {0x4C, 0x49, 0x56, 0x45, 0x00}, // LIVE
        {0x52, 0x41, 0x4E, 0x44, 0x00}, // RAND
        {0x53, 0x52, 0x43, 0x00, 0x00}, // SRC
        {0x46, 0x58, 0x00, 0x00, 0x00}, // FX
        {0x49, 0x4E, 0x00, 0x00, 0x00}, // IN
        {0x4D, 0x46, 0x58, 0x00, 0x00}, // MFX
    };

    vec2 uv2 = uv1;

    uv2 *= 10;
    uv2.x -= 0.5;
    uv2.y += 0.5;

    vec2 uv3 = uv1 * 30;

    // base frame
    float f = 
        h_rect(uv2, vec2(-5, -2), vec2(1), 0.1) +
        h_rect(uv2, vec2(-2, -2), vec2(1), 0.1) +
        rect(uv2, vec2(-3.5, -2), vec2(0.5, 0.1)) +
        h_rect(uv2, vec2(-5, 2), vec2(1), 0.1) +
        h_rect(uv2, vec2(-2, 2), vec2(1), 0.1) +
        rect(uv2, vec2(-3.5, 2), vec2(0.5, 0.1)) +
        h_rect(uv2, vec2(2, 0), vec2(1), 0.1) +
        h_rect(uv2, vec2(5, 0), vec2(1), 0.1) +
        rect(uv2, vec2(3.5, 0), vec2(0.5, 0.1)) +
        rect(uv2, vec2(0.55, -2), vec2(1.5, 0.1)) +
        rect(uv2, vec2(2, -1.55), vec2(0.1, 0.55)) +
        rect(uv2, vec2(0.55, 2), vec2(1.5, 0.1)) +
        rect(uv2, vec2(2, 1.55), vec2(0.1, 0.55)) +
        rect(uv2, vec2(6.8, 0), vec2(0.75, 0.1)) +
        h_circle(uv2, vec2(7.8, 0), 0.3, 0.1)
        ;
    
    if (iDemo < 1 && iInputFormat1 == YUYV_FOURCC) {
        f += circle(uv2, vec2(-9, 2), 0.3);
    }

    if (iDemo < 1 && iInputFormat2 == YUYV_FOURCC) {
        f += circle(uv2, vec2(-9, -2), 0.3);
    }

    // show selected src/fx
    f += char_at(uv2, vec2(-5.4, 1.45), hex_chars[selected_srca]);
    f += random_srca ? circle(uv2, vec2(-4.25, 2.7), 0.1) : 0;
    f += char_at(uv2, vec2(-5.4, -2.55), hex_chars[selected_srcb]);
    f += random_srcb ? circle(uv2, vec2(-4.25, -1.3), 0.1) : 0;
    f += char_at(uv2, vec2(-2.4, 1.45), hex_chars[selected_fxa]);
    f += fxa_invert ? rect(uv2, vec2(-2, 2.7), vec2(0.5, 0.05)) : 0;
    f += random_fxa ? circle(uv2, vec2(-1.25, 2.7), 0.1) : 0;
    f += char_at(uv2, vec2(-2.4, -2.55), hex_chars[selected_fxb]);
    f += fxb_invert ? rect(uv2, vec2(-2, -1.3), vec2(0.5, 0.05)) : 0;
    f += random_fxb ? circle(uv2, vec2(-1.25, -1.3), 0.1) : 0;
    f += char_at(uv2, vec2(4.6, -0.55), hex_chars[selected_mfx]);
    f += mfx_invert ? rect(uv2, vec2(5, 0.7), vec2(0.5, 0.05)) : 0;
    f += random_mfx ? circle(uv2, vec2(5.75, 0.7), 0.1) : 0;

    // show current selected
    f += iSelected == 3 ? h_rect(uv2, vec2(-5, 2), vec2(1.2), 0.1) : 0;
    f += iSelected == 4 ? h_rect(uv2, vec2(-5, -2), vec2(1.2), 0.1) : 0;
    f += iSelected == 5 ? h_rect(uv2, vec2(-2, 2), vec2(1.2), 0.1) : 0;
    f += iSelected == 6 ? h_rect(uv2, vec2(-2, -2), vec2(1.2), 0.1) : 0;
    f += iSelected == 8 ? h_rect(uv2, vec2(5, 0), vec2(1.2), 0.1) : 0;

    // show selected src/fx
    f += iActive1 == 1 ? h_rect(uv2, vec2(-5, 0.8), vec2(1, 0), 0.1) : 0;
    f += iActive1 == 2 ? h_rect(uv2, vec2(-5, -3.2), vec2(1, 0), 0.1) : 0;
    f += iActive2 == 1 ? h_rect(uv2, vec2(-2, 0.8), vec2(1.2, 0), 0.1) : 0;
    f += iActive2 == 2 ? h_rect(uv2, vec2(-2, -3.2), vec2(1, 0), 0.1) : 0;
    f += iActive2 == 3 ? h_rect(uv2, vec2(5, -1.2), vec2(1, 0), 0.1) : 0;

    // show src/fx

    f += write_5(uv3 * 0.75, vec2(-11.6,-1.8), texts[6]);
    f += write_5(uv3 * 0.75, vec2(-4.2,-1.8), texts[7]);
    f += char_at(uv3 * 0.5, vec2(-11.5, 3.5), 0x41);
    f += char_at(uv3 * 0.5, vec2(-11.5, -6), 0x42);
    if (iDemo < 1 && (iInputFormat1 == YUYV_FOURCC || iInputFormat2 == YUYV_FOURCC)) {
        f += write_5(uv3 * 0.75, vec2(-19.7,-1.8), texts[8]);
    }
    f += write_5(uv3 * 0.75, vec2(10.9,2), texts[9]);

    // show inputs / feedback
    float line_a_a = rect(uv2, vec2(-7.5, 2), vec2(1.5, 0.1));
    float line_a_b = rect(uv2, vec2(-6.5, 2), vec2(0.5, 0.1)) + rect(uv2, vec2(-8.5, -2), vec2(0.5, 0.1)) + line(uv2, vec2(-7, 2.1), vec2(-8, -2.1), 0.2);
    float line_a_f = rect(uv2, vec2(-6.5, 2), vec2(0.5, 0.1)) + rect(uv2, vec2(0, 3.6), vec2(7, 0.1)) + rect(uv2, vec2(-7, 2.8), vec2(0.1, 0.9)) + rect(uv2, vec2(7, 1.8), vec2(0.1, 1.9));
    if (selected_srca == 5 && iInputFormat1 == YUYV_FOURCC) {
        f += line_a_a;
    } else if (selected_srca == 10 && iInputFormat2 == YUYV_FOURCC) {
        f += line_a_b;
    } else if (selected_srca % 5 == 0) {
        f += line_a_f;
    }

    float line_b_a = rect(uv2, vec2(-6.5, -2), vec2(0.5, 0.1)) + rect(uv2, vec2(-8.5, 2), vec2(0.5, 0.1)) + line(uv2, vec2(-7, -2.1), vec2(-8, 2.1), 0.2);
    float line_b_b = rect(uv2, vec2(-7.5, -2), vec2(1.5, 0.1));
    float line_b_f = rect(uv2, vec2(-6.5, -2), vec2(0.5, 0.1)) + rect(uv2, vec2(0, -3.6), vec2(7, 0.1)) + rect(uv2, vec2(-7, -2.8), vec2(0.1, 0.9)) + rect(uv2, vec2(7, -1.8), vec2(0.1, 1.9));
    if (selected_srcb == 5 && iInputFormat1 == YUYV_FOURCC) {
        f += line_b_a;
    } else if (selected_srcb == 10 && iInputFormat2 == YUYV_FOURCC) {
        f += line_b_b;
    } else if (selected_srcb % 5 == 0) {
        f += line_b_f;
    }

    // show page
    f += iPage == 0 ? circle(uv2, vec2(-0.75, -4.1), 0.3) : h_circle(uv2, vec2(-0.75, -4.1), 0.25, 0.1);
    f += iPage == 1 ? circle(uv2, vec2(0, -4.1), 0.3) : h_circle(uv2, vec2(0, -4.1), 0.20, 0.1);
    f += iPage == 2 ? circle(uv2, vec2(0.75, -4.1), 0.3) : h_circle(uv2, vec2(0.75, -4.1), 0.20, 0.1);

    // show fx values
    float fx_rect = 0;

    f = mix(f, 1 - f, rect(uv2, vec2(-2, 1.1 + 0.9 * fxa_value), vec2(0.9, 0.9 * fxa_value)));
    f = mix(f, 1 - f, rect(uv2, vec2(-2, -2.9 + 0.9 * fxb_value), vec2(0.9, 0.9 * fxb_value)));
    f = mix(f, 1 - f, rect(uv2, vec2(5, -0.9 + 0.9 * mfx_value), vec2(0.9, 0.9 * mfx_value)));

    // show mix
    f += char_at(uv2, vec2(1.55, -0.6), mix_type ? 0x4B : 0x4D);
    f = mix(f, 1 - f, rect(uv2, vec2(2, -0.9 + 0.9 * mix_value), vec2(0.9, 0.9 * mix_value)));

    // show debug info
    float v = 0;
    float x = 0;

    x = -15;
    f += write_5(uv3, vec2(x - 4.5,13), texts[1]);
    f += write_int(uv3, vec2(x + 1.5,13), int(iTempo), 3);
    v = fract(iBeats);
    f += h_rect(uv3, vec2(x, 12), vec2(4, 0.5), 0.2);
    f += rect(uv3, vec2(x + 4 * v - 4, 12), vec2(4 * v, 0.4));

    x = 15;
    if (iAutoRand > 0) {
        f += write_5(uv3, vec2(x - 4.5,13), texts[5]);
        f += write_int(uv3, vec2(x - 0.5,13), int(iAutoRandCycle), 5);
        v = fract(iBeats / iAutoRandCycle);
    } else {
        f += write_int(uv3, vec2(x - 0.5,13), int(iTime), 5);
        v = fract(iTime);
        f += write_5(uv3, vec2(x - 4.5,13), texts[2]);
    }
    f += h_rect(uv3, vec2(x, 12), vec2(4, 0.5), 0.2);
    f += rect(uv3, vec2(x + 4 * v - 4, 12), vec2(4 * v, 0.4));

    return vec4(f);
}

#endif