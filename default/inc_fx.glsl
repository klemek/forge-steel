#include inc_res.glsl
#include inc_magic.glsl
#include inc_functions.glsl

#ifndef INC_FX
#define INC_FX

vec4 fx_master(vec3 c0, vec3 c, int seed, vec3 m0) {
    float fx = magic(m0.xy, vec3(1,0,0), seed);
    bool invert = magic_trigger(vec3(m0.z, 0, 0), seed);

    vec3 c_out = mix(c0, c, fx);

    c_out = mix(c_out, 1 - c_out, invert ? 1 : 0);

    return vec4(c_out, 1.0);
}

subroutine vec4 fx_stage_sub(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0);

subroutine uniform fx_stage_sub fx_stage;

// FX 1 : thru
subroutine(fx_stage_sub) vec4 fx_1(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
{
    // start

	vec2 uv0 = vUV.st;

    // controls

    float hue = magic(f1, b1, seed + 10);
    float saturation = magic(f2, b2, seed + 20);
    float light = magic(f3, b3, seed + 30);

    // logic

    vec3 c0 = texture(previous, uv0).xyz;
    vec3 c = c0;
    c = shift3(c, hue);
    c *= 1 + saturation;
    c = mix(c + light * 2.0, c - (1 - light) * 2.0, step(0.5, light));

    return fx_master(c0, c, seed, m0);
}

// FX 2 : feedback + shift
vec4 fx_shift(vec2 vUV, sampler2D src0, sampler2D src1, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float zoom = magic(f1, b1, seed + 10);
    float x_shift = magic(f2, b2, seed + 20);
    float y_shift = magic(f3, b3, seed + 30);

    // logic
    
    vec3 c0 = texture(src0, uv0).xyz;
    
    vec2 uv2 = uv1;
    uv2 = mix(uv2 * (1 + zoom * 2), uv2 * (zoom), step(0.5, zoom));
    uv2 += vec2(x_shift * ratio, y_shift) * 2;
    vec3 c = reframe(src1, uv2).xyz;

    return fx_master(c0, c, seed, m0);
}

subroutine(fx_stage_sub) vec4 fx_2(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
{
    return fx_shift(vUV, previous, feedback, seed, b1, f1, b2, f2, b3, f3, m0);
}

// FX 3 : shift
subroutine(fx_stage_sub) vec4 fx_3(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
{
    return fx_shift(vUV, previous, previous, seed, b1, f1, b2, f2, b3, f3, m0);
}

// FX 4 : colorize
subroutine(fx_stage_sub) vec4 fx_4(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
{
    // start

	vec2 uv0 = vUV.st;

    // controls

    float c_black = magic(f1, b1, seed + 10);
    bool c_black_trigger = magic_trigger(b1, seed + 10);
    float c_white = magic(f2, b2, seed + 20);
    bool c_white_trigger = magic_trigger(b2, seed + 20);
    float delta = magic(f3, b3, seed + 30);

    // logic

    vec3 c0 = texture(previous, uv0).xyz;

    float f = mean(c0);
    float c_mix = mix(c_black, c_white, f) + delta;
    vec3 c = mix(c_black_trigger ? col(c_mix) : vec3(0), c_white_trigger ? col(c_mix) : vec3(1), f);

    return fx_master(c0, c, seed, m0);
}

// FX 5 : quantize
subroutine(fx_stage_sub) vec4 fx_5(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float pixel_size = magic(f1, b1, seed + 10);
    float quantize = magic(f2, b2, seed + 20);
    bool quantize_trigger = magic_trigger(b2, seed + 20);
    float blur = magic(f3, b3, seed + 30);

    // logic

    vec3 c0 = texture(previous, uv0).xyz;
    
    vec2 uv2 = uv1;
    float pixel = (1 - pixel_size) * 250 + 25;
    uv2 = round(uv2 * pixel) / pixel;
    vec3 c = gauss(previous, uv2 * vec2(1 / ratio, 1)  + .5, 3, 0.005 * blur).xyz;
    float colors = (1 - quantize) * 10 + 1;
    if (quantize_trigger) {
        c = round(c * colors) / colors;
    }
    // c = mix(c, 1 - c, step(noise_f(uv0 * 10 + vec2(iTime * 0.1, 0), 5), 0.5));

    return fx_master(c0, c, seed, m0);
}

// FX 6 : dithering
subroutine(fx_stage_sub) vec4 fx_6(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float pixel_size = magic(f1, b1, seed + 10);
    bool pixel_size_trigger = magic_trigger(b1, seed + 10);
    float quantize = magic(f2, b2, seed + 20);
    bool quantize_trigger = magic_trigger(b2, seed + 20);
    float blur = magic(f3, b3, seed + 30);

    // logic

    vec3 c0 = texture(previous, uv0).xyz;

    vec2 uv2 = uv1;
    float k1 = pow(2, 10 - floor(pixel_size * 5));
    float pixel = (1 - pixel_size) * 250 + 25;
    if (pixel_size_trigger) {
        uv2 = floor(uv2 * k1) / k1;
    }
    vec3 c = gauss(previous, uv2 * vec2(1 / ratio, 1)  + .5, 3, 0.005 * blur).xyz;
    float k3 = pow(2, 5 - floor(quantize * 5));
    if (quantize_trigger) {
        c *= k3;
        c = vec3(
            mix(floor(c.x), ceil(c.x), dither(uv2 * k1, c.x - floor(c.x))),
            mix(floor(c.y), ceil(c.y), dither(uv2 * k1, c.y - floor(c.y))),
            mix(floor(c.z), ceil(c.z), dither(uv2 * k1, c.z - floor(c.z)))
        );
        c /= k3;
    }

    return fx_master(c0, c, seed, m0);
}

// FX 7 : tv
subroutine(fx_stage_sub) vec4 fx_7(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float lens_v = magic(f1, b1, seed + 10);
    float horizontal_noise = magic(f2, b2, seed + 20);
    float zoom = magic(f3, b3, seed + 30);

    // logic
    
    vec3 c0 = texture(previous, uv0).xyz;
    
    vec2 uv2 = uv1;
    float k1 = lens_v * 0.5;
    uv2 *= 1 + zoom * 2;
    uv2 = lens(uv2, -k1, k1);
    float k = horizontal_noise;
    vec3 c = vec3(
        reframe_b(previous, uv2 + vec2((rand(uv0.y * 1000 + iTime) - 0.5) * k * 0.1)).x,
        reframe_b(previous, uv2 + vec2((rand(uv0.y * 1100 + iTime) - 0.5) * k * 0.1)).y,
        reframe_b(previous, uv2).z
    );

    return fx_master(c0, c, seed, m0);
}

// FX 8 : kaleidoscope
subroutine(fx_stage_sub) vec4 fx_8(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float axes = magic(f1, b1, seed + 10);
    float axes_trigger = magic_b(b1, seed + 10).x;
    float rotation = magic(f2, b2, seed + 20);
    float h_scroll = magic(f3, b3, seed + 30);

    // logic

    vec3 c0 = texture(previous, uv0).xyz;
    
    vec2 uv2 = uv1;
    uv2 = mix(uv2, kal2(uv2 * rot(0.25), floor(axes * 9 + 1)) * vec2(1, -2) + vec2(0, -0.5), axes_trigger);    
    uv2 *= rot(rotation);
    uv2.x = (saw(uv2.x / ratio + 0.5 + h_scroll * 2) - 0.5) * ratio;
    vec3 c = reframe(previous, uv2).xyz;

    return fx_master(c0, c, seed, m0);
}

#include inc_cp437.glsl

// FX 9 : cp437
subroutine(fx_stage_sub) vec4 fx_9(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
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
    float t = magic(seed + 40); 

    // logic

    vec3 c0 = texture(previous, uv0).xyz;
    
    vec2 uv2 = uv1;
    float k1 = 100 * (1 - zoom) + 10;
    float inv_k = 1 / k1;
    uv2 = floor(uv2 * k1) * inv_k;
    int start_char = charset_ctrl.x > 0 ? charsets[int(charset.x * CHARSETS) * 2] : 0x01;
    int char_span = int((charset_ctrl.x > 0 ? charsets[int(charset.x * CHARSETS) * 2 + 1] : 255));
    char_span = int(char_span * max(1 - charset.y, 1 / (char_span * 0.75)));
    ivec2 uv2i = ivec2(uv2 * k1);
    int code = ((charset_ctrl.y < 1 || (uv2i.x % 2 ^ uv2i.y % 2) > 0) ? 1 : 0) * (start_char + int((rand(uv2i) + char_delta) * char_span) % char_span);
    vec3 c = reframe(previous, uv2 + vec2(0, 0) * inv_k * 0.125).xyz * 0.2
        + reframe(previous, uv2 + vec2(1, 0) * inv_k * 0.125).xyz * 0.2
        + reframe(previous, uv2 + vec2(1, 1) * inv_k * 0.125).xyz * 0.2
        + reframe(previous, uv2 + vec2(0, 1) * inv_k * 0.125).xyz * 0.2
        + reframe(previous, uv2 + vec2(0.5, 0.5) * inv_k * 0.125).xyz * 0.2;
    c = char(mod(uv1 * k1, 1), code) ? c : vec3(0);

    return fx_master(c0, c, seed, m0);
}

// FX 10 : lens
subroutine(fx_stage_sub) vec4 fx_10(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float lens_v1 = magic(f1, b1, seed + 10);
    float lens_v2 = magic(f2, b2, seed + 20);
    float zoom = magic(f3, b3, seed + 30);
    float k = magic(seed + 40);

    // logic

    vec3 c0 = texture(previous, uv0).xyz;
    
    vec2 uv2 = uv1;
    uv2 *= 1 + zoom * 2;
    uv2 = lens(uv2, -lens_v2 * 10, lens_v1 * 10);
    vec3 c = reframe(previous, uv2).xyz;

    return fx_master(c0, c, seed, m0);
}

// FX 11 : spill
subroutine(fx_stage_sub) vec4 fx_11(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float wall1 = magic(f1, b1, seed + 10);
    float wall2 = magic(f2, b2, seed + 20);
    float angle = magic(f3, b3, seed + 30);

    // logic
    
    vec3 c0 = texture(previous, uv0).xyz;

    vec2 uv2 = uv1;
    uv2 *= rot(angle);
    uv2.y = min(uv2.y, 1 - wall1);
    uv2.y = -min(-uv2.y, 1 - wall2);
    uv2 *= rot(-angle);
    vec3 c = reframe(previous, uv2).xyz;

    return fx_master(c0, c, seed, m0);
}

// TODO FX 12
subroutine(fx_stage_sub) vec4 fx_12(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
{
    return fx_2(vUV, previous, feedback, seed, b1, f1, b2, f2, b3, f3, m0);
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    // logic
    
    vec3 c0 = texture(previous, uv0).xyz;
    vec3 c = c0;

    return fx_master(c0, c, seed, m0);
}

// TODO FX 13
subroutine(fx_stage_sub) vec4 fx_13(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
{
    return fx_3(vUV, previous, feedback, seed, b1, f1, b2, f2, b3, f3, m0);
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    // logic
    
    vec3 c0 = texture(previous, uv0).xyz;
    vec3 c = c0;

    return fx_master(c0, c, seed, m0);
}

// TODO FX 14
subroutine(fx_stage_sub) vec4 fx_14(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
{
    return fx_4(vUV, previous, feedback, seed, b1, f1, b2, f2, b3, f3, m0);
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    // logic

    vec3 c0 = texture(previous, uv0).xyz;
    vec3 c = c0;

    return fx_master(c0, c, seed, m0);
}

// TODO FX 15
subroutine(fx_stage_sub) vec4 fx_15(vec2 vUV, sampler2D previous, sampler2D feedback, int seed, vec3 b1, vec2 f1, vec3 b2, vec2 f2, vec3 b3, vec2 f3, vec3 m0)
{
    return fx_5(vUV, previous, feedback, seed, b1, f1, b2, f2, b3, f3, m0);
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    // logic
    
    vec3 c0 = texture(previous, uv0).xyz;
    vec3 c = c0;

    return fx_master(c0, c, seed, m0);
}

#endif