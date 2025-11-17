#include inc_res.glsl
#include inc_magic.glsl
#include inc_functions.glsl

#ifndef INC_SRC
#define INC_SRC

uniform int iDemo;

uniform sampler2D iTex0;
uniform sampler2D iTex3;
uniform sampler2D iTex4;

subroutine vec4 src_stage_sub(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3);

vec4 src_thru(vec2 vUV, sampler2D tex, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
    // start

	vec2 uv0 = vUV.st;

    // controls

    float hue = magic(f1, b1, seed + 10);
    float saturation = magic(f2, b2, seed + 20);
    float light = magic(f3, b3, seed + 30);

    // logic

    vec3 c = texture(tex, uv0).xyz;

    c = shift3(c, hue);

    c *= 1 + saturation;
    c = mix(c + light * 2.0, c - (1 - light) * 2.0, step(0.5, light));

    // output

   return vec4(c, 1.);
}

// SRC 1: feedback + thru
subroutine(src_stage_sub) vec4 src_1(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
    return src_thru(vUV, iTex0, seed, b1, f1, b2, f2, b3, f3);
}

// SRC 2 : lines
subroutine(src_stage_sub) vec4 src_2(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float thickness = magic(f1, b1, seed + 10);
    float rotation = magic(f2, b2, seed + 20);
    float distort = magic(f3, b3, seed + 30);

    // logic

    vec2 uv2 = uv1;
    uv2.y *= cos(uv2.x * 5 * distort);
    uv2 *= rot(rotation + iBeats / 16);
    float k = thickness * 2;
    uv2.y = cmod(uv2.y, k * 2 + 0.1);
    float f = istep(k * 0.125 + 0.05, uv2.y) * istep(k * 0.125 + 0.01, -uv2.y);
    
    return vec4(f);
}

// SRC 3 : dots
subroutine(src_stage_sub) vec4 src_3(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float zoom = magic(f1, b1, seed + 10);
    float rotation = magic(f2, b2, seed + 20);
    float lens_v = magic(f3, b3, seed + 30);

    // logic

    vec2 uv2 = uv1;
    float k1 = lens_v * 5;
    uv2 = lens(uv2, -k1, k1);
    uv2 = kal(uv2, 5);
    uv2 *= rot(rotation + iBeats / 16);
    float k = zoom * 0.1 + 0.05;
    uv2 = cmod(uv2, k * 2);
    float f = istep(k / (1 + length(uv1) * 2), length(uv2));
    
    return vec4(f);
}

// SRC 4 : circuit
subroutine(src_stage_sub) vec4 src_4(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float z = 10 + magic(f1, b1, 123) * 20;
    float h = magic(f2, b2, seed + 20) * 0.8 + 0.1;
    float v = magic_reverse(f3, b3, seed + 30) * 0.8 + 0.1;

    // logic

    uv1 *= z;
    uv1 += iBeats;

    float s0 = rand(floor(mod(uv1, 1000))) * 1000;
    float s1 = rand(floor(mod(uv1 + vec2(0, 1), 1000))) * 1000;
    float s2 = rand(floor(mod(uv1 - vec2(1, 0), 1000))) * 1000;

    bool up = rand(s1 + 1) < h;
    bool left = rand(s2 + 2) < v;
    bool down = rand(s0 + 1) < h;
    bool right = rand(s0 + 2) < v;
    bool up_down = up && down;
    bool left_right = left && right;

    uv1 = mod(uv1, 1.0) - 0.5;

    const float t = 0.1;

    float f = 0;
    int c = 0;

    if (up) {
        f += stripe(uv1.x, -t * 0.5, t * 0.5) * step(-t * 0.5, uv1.y);
        c += 1;
    }

    if (down) {
        f += stripe(uv1.x, -t * 0.5, t * 0.5) * istep(t * 0.5, uv1.y);
        c += 1;
    }

    if (left) {
        f += stripe(uv1.y, -t * 0.5, t * 0.5) * istep(t * 0.5, uv1.x);
        c += 1;
    }

    if (right) {
        f += stripe(uv1.y, -t * 0.5, t * 0.5) * step(-t * 0.5, uv1.x);
        c += 1;
    }

    if (c == 1) {
        f += istep(t, length(uv1));
    }

    f = min(f, 1);

    if ((up_down ^^ left_right) && c == 2) {
        if (up_down) {
            uv1.xy = uv1.yx;
        }
        if (rand(s0 + 3) < 0.5) {
            uv1.x = -uv1.x;
        }
        float k = rand(s0 + 4) * 60;
        f -= rect(uv1, vec2(0), vec2(t * 3, t));
        f = max(0, f);
        if (k < 10) { // resistor
            f += line(uv1, vec2(-t * 3.25, -t * 0.5), vec2(-t * 2.5, t * 2), t * 0.75);
            f += line(uv1, vec2(-t * 2.5, t * 2), vec2(-t * 1.5, -t * 2), t * 0.75);
            f += line(uv1, vec2(-t * 1.5, -t * 2), vec2(-t * 0.5, t * 2), t * 0.75);
            f += line(uv1, vec2(-t * 0.5, t * 2), vec2(t * 0.5, -t * 2), t * 0.75);
            f += line(uv1, vec2(t * 0.5, -t * 2), vec2(t * 1.5, t * 2), t * 0.75);
            f += line(uv1, vec2(t * 1.5, t * 2), vec2(t * 2.5, -t * 2), t * 0.75);
            f += line(uv1, vec2(t * 2.5, -t * 2), vec2(t * 3.25, t * 0.5), t * 0.75);
        } else if (k < 20) { // capacitor
            f += rect(uv1, vec2(-t * 2, 0), vec2(t, t * 0.5));
            f += rect(uv1, vec2(t * 2, 0), vec2(t, t * 0.5));
            f += rect(uv1, vec2(t, 0), vec2(t * 0.5, t * 3.5));
            f += rect(uv1, vec2(-t, 0), vec2(t * 0.5, t * 3.5));
        } else if (k < 30) { // diode
            f += line(uv1, vec2(-t * 2, t * 2.5), vec2(t * 2, 0), t);
            f += line(uv1, vec2(-t * 2, -t * 2.5), vec2(t * 2, 0), t);
            f += rect(uv1, vec2(t * 2.5, 0), vec2(t * 0.5, t * 3));
            f += rect(uv1, vec2(-t * 2.5, 0), vec2(t * 0.5, t * 3));
        } else if (k < 40) { // lamp
            f += istep(t * 3.5, length(uv1));
            f -= istep(t * 2.5, length(uv1));
            f += line(uv1, vec2(-t * 2), vec2(t * 2), t);
            f += line(uv1, vec2(-t * 2, t * 2), vec2(t * 2, -t * 2), t);
        } else if (k < 50) { // inductor
            f += istep(t * 2, length(uv1 - vec2(t * 2.5,0)));
            f += istep(t * 2, length(uv1 - vec2(0,0)));
            f += istep(t * 2, length(uv1 - vec2(-t * 2.5,0)));
            f -= 2 * istep(t, length(uv1 - vec2(t * 2.5,0)));
            f -= 2 * istep(t, length(uv1 - vec2(0,0)));
            f -= 2 * istep(t, length(uv1 - vec2(-t * 2.5,0)));
            f *= step(-t * 0.5, uv1.y);
        } else if (k < 60) { // switch
            f += istep(t, length(uv1 - vec2(t * 2.5, 0)));
            f += istep(t, length(uv1 + vec2(t * 2.5, 0)));
            f += line(uv1, vec2(t * 2, 0), vec2(-t * 2.5, t * (k < 55 ? 3 : 1)), t);
        }
    } else if (c == 3) {
        if (left_right) {
            uv1.xy = uv1.yx;
            if (up) {
                uv1.x = -uv1.x;
            }
        } else if (right) {
            uv1.x = -uv1.x;
        }
        float k = rand(s0 + 4) * 20;
        if (k < 10) {
            f -= rect(uv1, vec2(0), vec2(t * 3));
            f = max(0, f);
            f += rect(uv1, vec2(-t * 3, 0), vec2(t * 0.5, t * 3));
            f += line(uv1, vec2(t * 0.25, t * 3.25), vec2(-t * 3, t), t);
            f += line(uv1, vec2(t * 0.25, -t * 3.25), vec2(-t * 3, -t), t);
        }
    }
    
    return vec4(f);
}

// SRC 5 : noise
subroutine(src_stage_sub) vec4 src_5(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float zoom = magic(f1, b1, seed + 10);
    float voronoi_distort = magic(f2, b2, seed + 20);
    float details = magic(f3, b3, seed + 30);
    float noise_factor = magic(seed + 40);

    // logic

    vec2 uv2 = uv1;
    uv2 *= zoom * 20 + 3;
    uv2.x += iBeats;
    vec4 data = voronoi(uv2, voronoi_distort);
    float f = data.x / (data.x + data.y);
    f = sin(f * PI * (details * 20)) * 0.5 + 1;
    int nf = int(noise_factor * 6);
    f *= mix(1, noise_f(uv2, nf - 1), step(0.0, float(nf)));
    
    return vec4(f);
}

// SRC 6 : video in 1 + thru
subroutine(src_stage_sub) vec4 src_6(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
    if (iDemo > 0) {
        return src_2(vUV, seed, b1, f1, b2, f2, b3, f3);
    }

    return src_thru(vUV, iTex3, seed, b1, f1, b2, f2, b3, f3);
}

#include inc_cp437.glsl

// SRC 7 : cp437
subroutine(src_stage_sub) vec4 src_7(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float zoom = magic(f1, b1, seed + 10);
    vec2 charset = magic_f(f2, b2, seed + 20);
    vec3 charset_ctrl = magic_b(b2, seed + 20);
    float char_delta = magic(f3, b3, seed + 30);

    // logic

    vec2 uv2 = uv1;
    uv2 *= zoom * 20 + 3;
    uv2 += iBeats;
    uv2 = mod(uv2, 100) + 100;
    int start_char = charset_ctrl.x > 0 ? charsets[int(charset.x * CHARSETS) * 2] : 0x01;
    int char_span = int((charset_ctrl.x > 0 ? charsets[int(charset.x * CHARSETS) * 2 + 1] : 255));
    char_span = int(char_span * max(1 - charset.y, 1 / (char_span * 0.75)));
    ivec2 uv2i = ivec2(uv2);
    int code = ((charset_ctrl.y < 1 || (uv2i.x % 2 ^ uv2i.y % 2) > 0) ? 1 : 0) * (start_char + int((rand(uv2i) + char_delta) * char_span) % char_span);
    uv2 = mod(uv2, 1);
    float f = char(uv2, code) ? 1 : 0;
    
    return vec4(f);
}

// SRC 8 : sentences

#include inc_sentences.glsl

subroutine(src_stage_sub) vec4 src_8(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float zoom = magic(f1, b1, seed + 10);
    float sentence = magic_reverse(f2, b2, seed + 20);
    float h_delta = magic(f3, b3, seed + 30);
    vec3 h_delta_b = magic_b(b3, seed + 30);

    // logic

    vec2 uv2 = uv1;
    uv2 *= (1 + zoom) * 12;
    uv2.y += 0.5;
    int s = int(sentence * (SENTENCE_COUNT - 1));
    uv2.x += floor(uv2.y) * (h_delta - 0.5) * 2;
    uv2.y = mix(uv2.y, mod(uv2.y, 1), h_delta_b.x);
    float f = write_20(uv2, vec2(-float(lengths[s]) * 0.5, 0), sentences[s]);
    
    return vec4(f);
}

// SRC 9 : sentences repeat
subroutine(src_stage_sub) vec4 src_9(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
     // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float h_shift = magic(f1, b1, seed + 10);
    float sentence = magic_reverse(f2, b2, seed + 20);
    float h_delta = magic(f3, b3, seed + 30);
    vec3 h_delta_b = magic_b(b3, seed + 30);

    // logic

    vec2 uv2 = uv1;
    uv2.x += h_shift;
    uv2 *= 15;
    uv2.y += 0.5;
    int s = int(sentence * (SENTENCE_COUNT - 1));
    float slen = float(lengths[s]);
    uv2.x += floor(uv2.y) * (h_delta - 0.5) * 2;
    uv2.x = cmod(uv2.x, slen + 1);
    vec2 uv3 = uv2;
    uv3.y = mix(uv3.y, mod(uv3.y, 1), h_delta_b.x);
    float f = write_20(uv3, vec2(-slen * 0.5, 0), sentences[s]);

    f *= (1 - abs(floor(uv2.y) * 0.125)) * (1 - abs(floor(uv2.y) * 0.125));

    return vec4(f);
}

// SRC 10 : isometric grid
subroutine(src_stage_sub) vec4 src_10(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float zoom = 5 + magic(f1, b1, seed + 10) * 15;
    float h_scroll = magic(f2, b2, seed + 20);
    float max_elevation = magic(f3, b3, seed + 30) * 0.5;
    float thick = 0.1;

    // logic

    vec2 uv2 = uv1;

    vec2 uv3 = iso(uv2);

    uv3 += vec2(h_scroll, 0);

    uv3 *= round(zoom);

    vec2 umax = vec2(round(zoom), 300);

    vec2 u0 = mod(floor(uv3), umax);
    vec2 u1 = mod(floor(uv3 + vec2(1, 0)), umax);
    vec2 u2 = mod(floor(uv3 + vec2(0, 1)), umax);
    vec2 u3 = mod(floor(uv3 + vec2(-1, 0)), umax);
    vec2 u4 = mod(floor(uv3 + vec2(0, -1)), umax);
    
    float e0 = (rand(floor(u0)) * 2 - 1) * max_elevation;
    float e1 = (rand(floor(u1)) * 2 - 1) * max_elevation;
    float e2 = (rand(floor(u2)) * 2 - 1) * max_elevation;
    float e3 = (rand(floor(u3)) * 2 - 1) * max_elevation;
    float e4 = (rand(floor(u4)) * 2 - 1) * max_elevation;

    uv3 = mod(uv3, 1.0) - 0.5;

    float f = 0;

    f = line(uv3, vec2(0, 0) - iso_z(e0), vec2(1, 0) - iso_z(e1), thick)
        + line(uv3, vec2(0, 0) - iso_z(e0), vec2(0, 1) - iso_z(e2), thick)
        + line(uv3, vec2(0, 0) - iso_z(e0), vec2(-1, 0) - iso_z(e3), thick)
        + line(uv3, vec2(0, 0) - iso_z(e0), vec2(0, -1) - iso_z(e4), thick);
    
    return vec4(f);
}

// SRC 11 : video in 2 + thru
subroutine(src_stage_sub) vec4 src_11(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
    if (iDemo > 0) {
        return src_3(vUV, seed, b1, f1, b2, f2, b3, f3);
    }

    return src_thru(vUV, iTex4, seed, b1, f1, b2, f2, b3, f3);
}

// SRC 12 : Scales
subroutine(src_stage_sub) vec4 src_12(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float zoom = 5 + magic(f1, b1, seed + 10) * 15;
    float shape = magic(f2, b2, seed + 20);
    float repeat = 1 + magic(f3, b3, seed + 30) * 10;

    // logic

    float f = 0;

    vec2 uv2 = uv1;

    uv2 *= zoom;

    uv2 = mod(uv2, 4);

    vec2 uv3 = uv2;

    uv3.y = mix(uv3.y, -uv3.y, step(2, uv2.x) * step(2, uv2.y));
    uv3.y = mix(uv3.y, -uv3.y, istep(2, uv2.x) * istep(2, uv2.y));

    uv3.y = -uv3.y;

    uv3.x = mix(-uv3.x, uv3.x, istep(3, uv2.x) * step(1, uv2.x));
    uv3.x = mix(-uv3.x, uv3.x, istep(3, uv2.y) * step(1, uv2.y));

    f = istep(0.5, saw((length(mod(uv3, 1)) + shape + 0.5) * repeat));

    f = mix(1 - f, f, istep(1, abs(uv2.y - 2)) * istep(2, uv2.x));
    f = mix(1 - f, f, step(1, abs(uv2.y - 2)) * step(2, uv2.x));

    return vec4(f);
}

// SRC 13 : Credenza
subroutine(src_stage_sub) vec4 src_13(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float zoom = 5 + magic(f1, b1, seed + 10) * 15;
    float shape = 0.1 + magic(f2, b2, seed + 20) * 0.9;
    float repeat = 1 + magic(f3, b3, seed + 30) * 10;

    // logic

    vec2 uv2 = uv1;

    uv2 *= zoom;
    
    uv2 = mod(uv2, 2);

    uv2 = abs(uv2 - 1);

    float f = istep(1, length(uv2)) * istep(1, length(1 - uv2));

    f *= istep(0.5, saw((length(uv2) + shape + 0.5) * repeat)) * istep(0.5, saw((length(1 - uv2) + shape + 0.5) * repeat));
    
    return vec4(f);
}

// SRC 14 : Cursor
subroutine(src_stage_sub) vec4 src_14(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float size = magic(f1, b1, seed + 10) * 2 + 0.1;
    float dx = (fmagic(f2, b2, seed + 20) * (1 - size * 0.06) - 0.5) * ratio;
    bool flip_x = magic_trigger(b2, seed + 20);
    float dy = fmagic(f3, b3, seed + 30) * (1 - size * 0.17) - 0.5;
    bool flip_y = magic_trigger(b3, seed + 30);

    // logic

    vec2 uv2 = uv1;

    if (flip_x) {
        uv2.x = -uv2.x;
    }

    if (flip_y) {
        uv2.y = -uv2.y;
    }

    uv2 -= vec2(dx, -dy);

    uv2 /= size;

    uv2 *= -rot(-PI / 2);

    float f = istep(0, (uv2.x * 2 + uv2.y) * (uv2.x * 2 - uv2.y)) * istep(0, uv2.y);

    uv2.y += 0.1;

    f  -= istep(0, (uv2.x + uv2.y * 3) * (uv2.x - uv2.y * 3)) * istep(0, uv2.y);

    f += rect(uv2, vec2(0, -0.04), vec2(0.01, 0.04));
    
    return vec4(f);
}

// SRC 15 : Random
subroutine(src_stage_sub) vec4 src_15(vec2 vUV, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3)
{
    int src = int(randTime(seed + 100) * 14);

    if (src == 0) {
        return src_1(vUV, seed, b1, f1, b2, f2, b3, f3);
    } else if (src == 1) {
        return src_2(vUV, seed, b1, f1, b2, f2, b3, f3);
    } else if (src == 2) {
        return src_3(vUV, seed, b1, f1, b2, f2, b3, f3);
    } else if (src == 3) {
        return src_4(vUV, seed, b1, f1, b2, f2, b3, f3);
    } else if (src == 4) {
        return src_5(vUV, seed, b1, f1, b2, f2, b3, f3);
    } else if (src == 5) {
        return src_6(vUV, seed, b1, f1, b2, f2, b3, f3);
    } else if (src == 6) {
        return src_7(vUV, seed, b1, f1, b2, f2, b3, f3);
    } else if (src == 7) {
        return src_8(vUV, seed, b1, f1, b2, f2, b3, f3);
    } else if (src == 8) {
        return src_9(vUV, seed, b1, f1, b2, f2, b3, f3);
    } else if (src == 9) {
        return src_10(vUV, seed, b1, f1, b2, f2, b3, f3);
    } else if (src == 10) {
        return src_11(vUV, seed, b1, f1, b2, f2, b3, f3);
    } else if (src == 11) {
        return src_12(vUV, seed, b1, f1, b2, f2, b3, f3);
    } else if (src == 12) {
        return src_13(vUV, seed, b1, f1, b2, f2, b3, f3);
    } else {
        return src_14(vUV, seed, b1, f1, b2, f2, b3, f3);
    }
}

#endif