#ifndef PI
#define PI 3.1415927
#endif

#include inc_res.glsl

#ifndef INC_FUNCTIONS
#define INC_FUNCTIONS

// BASICS

float istep(float x, float y) {
    return 1 - step(x, y);
}

float estep(float x, float y) {
    return smoothstep(x - 0.0001, x + 0.0001, y);
}

float iestep(float x, float y) {
    return 1 - estep(x, y);
}

float ease(float x) {
    return 0.5 - cos(max(min(x, 1.0), 0.0)*PI) * 0.5;
}

vec2 ease(vec2 x) {
    return 0.5 - cos(max(min(x, 1.0), 0.0)*PI) * 0.5;
}

vec3 ease(vec3 x) {
    return 0.5 - cos(max(min(x, 1.0), 0.0)*PI) * 0.5;
}

float saw(float x){
    return abs(mod(x+1,2)-1);
}

vec2 saw(vec2 x){
    return abs(mod(x+1,2)-1);
}

vec3 saw(vec3 x){
    return abs(mod(x+1,2)-1);
}

float cmod(float x, float k){
    return mod(x + k * 0.5, k) - k * 0.5;
}

vec2 cmod(vec2 x, float k){
    return mod(x + k * 0.5, k) - k * 0.5;
}

vec3 cmod(vec3 x, float k){
    return mod(x + k * 0.5, k) - k * 0.5;
}

// COLORS

vec3 col(float x){
    return vec3(
        .5*(sin(x*2.*PI)+1.),
        .5*(sin(x*2.*PI+2.*PI/3.)+1.),
        .5*(sin(x*2.*PI-2.*PI/3.)+1.)
    );
}

vec3 shift(vec3 c, float f) {
    return vec3(
        c.x * (1 - f) + c.y * f,
        c.y * (1 - f) + c.z * f,
        c.z * (1 - f) + c.x * f
    );
}

vec3 shift3(vec3 c, float f) {
    return shift(shift(shift(c, f), f), f);
}

vec3 mix3(vec3 c1, vec3 c2, vec3 c3, float x) {
    return istep(0.5, x) * mix(c1, c2, x * 2)
        + step(0.5, x) * mix(c2, c3, x * 2 - 1)
    ;
}

vec3 mix4(vec3 c1, vec3 c2, vec3 c3, vec3 c4, float x) {
    return istep(0.333, x) * mix(c1, c2, x * 3)
        + step(0.333, x) * istep(0.667, x) * mix(c2, c3, x * 3 - 1)
        + step(0.667, x) * mix(c3, c4, x * 3 - 2);
    ;
}

vec3 mix5(vec3 c1, vec3 c2, vec3 c3, vec3 c4, vec3 c5, float x) {
    return istep(0.25, x) * mix(c1, c2, x * 4)
        + step(0.25, x) * istep(0.5, x) * mix(c2, c3, x * 4 - 1)
        + step(0.5, x) * istep(0.75, x) * mix(c3, c4, x * 4 - 2)
        + step(0.75, x) * mix(c4, c5, x * 4 - 3)
    ;
}

vec3 mix6(vec3 c1, vec3 c2, vec3 c3, vec3 c4, vec3 c5, vec3 c6, float x) {
    return istep(0.2, x) * mix(c1, c2, x * 5)
        + step(0.2, x) * istep(0.4, x) * mix(c2, c3, x * 5 - 1)
        + step(0.4, x) * istep(0.6, x) * mix(c3, c4, x * 5 - 2)
        + step(0.6, x) * istep(0.8, x) * mix(c4, c5, x * 5 - 3)
        + step(0.8, x) * mix(c5, c6, x * 5 - 4)
    ;
}

float mean(vec3 v)
{
    return v.x * 0.3333 + v.y * 0.3333 + v.z * 0.3333;
}

float mean(vec4 v)
{
    return v.x * 0.3333 + v.y * 0.3333 + v.z * 0.3333;
}

// OTHER

mat2 rot(float angle){
    return mat2(
        cos(angle*2.*PI),-sin(angle*2.*PI),
        sin(angle*2.*PI),cos(angle*2.*PI)
    );
}

// EFFECTS

vec2 lens(vec2 uv, float limit, float power) {
    return uv * (1 + limit + length(uv * power));
}

vec2 kal(vec2 uv, float n) {
    float t = atan(uv.y, uv.x) + PI * 0.5;
    float q = 3.0 / (2.0 * PI);
    t = abs(mod(t + PI / (n), 2 * PI / n) - PI / (n));
    return length(uv) * vec2(
        cos(t),
        sin(t)
    );
}

vec2 kal2(vec2 uv, float n) {
    float t = atan(uv.y, uv.x) + PI * 0.5;
    float t2 = abs(mod(t + PI / n, 2 * PI / n) - PI / n);
    return length(uv) * vec2(
        cos(t2),
        sin(t2)
    );
}

// NOISE

float noise(vec2 n)
{
    const vec2 d = vec2(0, 1);
    vec2 b = floor(n);
    vec2 f = fract(n);
    f *= f * (3 - 2 * f);
    return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

float noise_f(vec2 n, int f)
{
    float o = noise(n) / 2;
    o += mix(o, noise(n * 2), f > 1 ? 1 : 0) / 4;
    o += mix(o, noise(n * 4), f > 2 ? 1 : 0) / 8;
    o += mix(o, noise(n * 8), f > 3 ? 1 : 0) / 16;
    o += mix(o, noise(n * 16), f > 4 ? 1 : 0) / 32;
    return o;
}

// VORONOI

float v_index(vec2 uv) {
    return floor(uv.x) + floor(uv.y) * 45;
}

vec2 v_pos(float i) {
    int iTimeId = int(iBeats);
    float iTimeV = iBeats - iTimeId;

    float x0 = rand(i + 823 + iTimeId);
    float y0 = rand(i + 328 + iTimeId);
    
    float x1 = rand(i + 823 + iTimeId + 1);
    float y1 = rand(i + 328 + iTimeId + 1);
    
    return vec2(
        mix(x0, x1, ease(ease(iTimeV))),
        mix(y0, y1, ease(ease(iTimeV)))
    );
}

vec4 voronoi(vec2 uv, float dist) {
    vec4 o = vec4(0, 0, 2, 0);
    vec4 t = vec4(0, 0, 2, 0);
    float d, i;
    vec2 uv2, p;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            uv2 = vec2(floor(uv.x) + dx, floor(uv.y) + dy);
            i = v_index(uv2);
            p = uv2 + v_pos(i) * dist;
            d = length(p - uv);
            if (d < o.z) {
                t = o;
                o = vec4(p, d, i);
            } else if (d < t.z) {
                t = vec4(p, d, i);
            }
        }
    }
    return vec4(o.z, t.z, o.w, t.w);
}

// SHAPES

float stripe(float x, float k1, float k2)
{
    return k2 > k1 ? (1 - step(x, k1)) * (step(x, k2)) : ((1 - step(x, k2)) * (step(x, k1)));
}

float capsule(vec2 uv, float r, float d, float a)
{
    uv *= rot(a);
    float f1 = step(uv.x, r) * step(-uv.x, r) * step(uv.y, d - r) * step(-uv.y, d - r);
    float f2 = step(length(uv - vec2(0, d - r)), r);
    float f3 = step(length(uv + vec2(0, d - r)), r);
    return min(1, f1 + f2 + f3);
}

float rect(vec2 uv, vec2 c, vec2 size) {
    uv -= c;
    return step(abs(uv.x), size.x) * step(abs(uv.y), size.y);
}

float h_rect(vec2 uv, vec2 c, vec2 size, float k) {
    return rect(uv, c, size + k * 0.5) - rect(uv, c, size - k * 0.5);
}

float line(vec2 uv, vec2 p1, vec2 p2, float thick) {
    vec2 p = p2 - p1;
    uv -= p1;
    vec2 k;
    
    if (abs(p.y) > abs(p.x)) {
        k = vec2(
            uv.x - p.x * uv.y / p.y,
            uv.y / p.y
        );
        
        return step(k.x, thick * 0.5)
            * step(-k.x, thick * 0.5)
            * step(k.y, 1)
            * (1 - step(k.y, 0));
    } else {
        k = vec2(
            uv.x / p.x,
            uv.y - p.y * uv.x / p.x
        );
        
        return step(k.y, thick * 0.5)
            * step(-k.y, thick * 0.5)
            * step(k.x, 1)
            * (1 - step(k.x, 0));
    }
}

const mat2x2 ISOMETRIC_MATRIX = {{0.5, 1}, {0.5, -1}};

vec2 iso(vec2 p) {
    return p * ISOMETRIC_MATRIX;
}

vec2 iso_z(float z) {
    return vec2(
        -z,
        z
    );
}

vec2 iso(vec3 uv) {
    return iso(uv.xy) + iso_z(uv.z);
}

// INPUTS

vec4 reframe(sampler2D tex, vec2 uv)
{
    uv = uv * vec2(iResolution.y / iResolution.x, 1)  + .5;
    uv = saw(uv);
    return texture(tex, uv);
}

vec4 reframe_b(sampler2D tex, vec2 uv)
{
    uv = uv * vec2(iResolution.y / iResolution.x, 1)  + .5;
    return texture(tex, uv);
}

vec4 kernel(sampler2D tex, vec2 uv, mat3x3 k, float spm)
{
    int x, y;
    vec2 offset;
    vec4 sum = vec4(0);
    for (y = -1; y <= 1; ++y) {
        for (x = -1; x <= 1; ++x) {
            offset = vec2(x, y) * spm;
            if (abs(k[x + 1][y + 1]) > 0) {
                sum += k[x + 1][y + 1] * texture(tex, uv + offset);
            }
        }
    }
    return sum;
}

// BLUR

float gaussian_weight(float x, float sigma)
{
    return exp(-(x * x) / (2.0 * sigma * sigma));
}

vec4 gauss(sampler2D tex, vec2 uv, const int kernel_size, float spm, float sigma, float bloom)
{
    int x, y;
    vec2 offset;
    float w;
    vec4 sum = vec4(0);
    float weight_sum = 0;
    for (y = -kernel_size; y <= kernel_size; ++y) {
        for (x = -kernel_size; x <= kernel_size; ++x) {
            offset = vec2(x, y) * spm;
            w = gaussian_weight(length(vec2(x, y)), sigma);
            sum += texture(tex, uv + offset) * w;
            weight_sum += w;
        }
    }
    return (sum / weight_sum) * bloom;
}

vec4 gauss(sampler2D tex, vec2 uv, const int kernel_size, float spm, float sigma)
{
    return gauss(tex, uv, kernel_size, spm, sigma, 1.0);
}

vec4 gauss(sampler2D tex, vec2 uv, const int kernel_size, float spm)
{
    return gauss(tex, uv, kernel_size, spm, 2.0);
}

vec4 gauss(sampler2D tex, vec2 uv, const int kernel_size)
{
    return gauss(tex, uv, kernel_size, 1 / max(iResolution.x, iResolution.y));
}

vec4 gauss(sampler2D tex, vec2 uv)
{
    return gauss(tex, uv, 3);
}

float dither(vec2 uv, float x)
{
    bool o = false;
    o = o || x * 16 >= 1 && floor(mod(uv.x, 4)) == 0 && floor(mod(uv.y, 4)) == 0;
    o = o || x * 16 >= 2 && floor(mod(uv.x, 4)) == 2 && floor(mod(uv.y, 4)) == 2;
    o = o || x * 16 >= 3 && floor(mod(uv.x, 4)) == 0 && floor(mod(uv.y, 4)) == 2;
    o = o || x * 16 >= 4 && floor(mod(uv.x, 4)) == 2 && floor(mod(uv.y, 4)) == 0;
    o = o || x * 16 >= 5 && floor(mod(uv.x, 4)) == 1 && floor(mod(uv.y, 4)) == 1;
    o = o || x * 16 >= 6 && floor(mod(uv.x, 4)) == 3 && floor(mod(uv.y, 4)) == 3;
    o = o || x * 16 >= 7 && floor(mod(uv.x, 4)) == 1 && floor(mod(uv.y, 4)) == 3;
    o = o || x * 16 >= 8 && floor(mod(uv.x, 4)) == 3 && floor(mod(uv.y, 4)) == 1;
    o = o || x * 16 >= 9 && floor(mod(uv.x, 4)) == 1 && floor(mod(uv.y, 4)) == 0;
    o = o || x * 16 >= 10 && floor(mod(uv.x, 4)) == 3 && floor(mod(uv.y, 4)) == 2;
    o = o || x * 16 >= 11 && floor(mod(uv.x, 4)) == 1 && floor(mod(uv.y, 4)) == 2;
    o = o || x * 16 >= 12 && floor(mod(uv.x, 4)) == 3 && floor(mod(uv.y, 4)) == 0;
    o = o || x * 16 >= 13 && floor(mod(uv.x, 4)) == 0 && floor(mod(uv.y, 4)) == 1;
    o = o || x * 16 >= 14 && floor(mod(uv.x, 4)) == 2 && floor(mod(uv.y, 4)) == 3;
    o = o || x * 16 >= 15 && floor(mod(uv.x, 4)) == 0 && floor(mod(uv.y, 4)) == 3;
    o = o || x * 16 >= 16 && floor(mod(uv.x, 4)) == 2 && floor(mod(uv.y, 4)) == 1;
    return o ? 1 : 0;
}

#endif
