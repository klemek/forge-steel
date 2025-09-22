#version 460

// COMMON DEFINITIONS AND FUNCTIONS
// ================================

// 1. real time uniforms
// ---------------------

uniform float iTime;
uniform float iTempo;
uniform int iFPS;
uniform vec2 iResolution;
uniform vec2 iTexResolution;
uniform int iDemo;

uniform int seed1;
uniform int seed2;
uniform int seed3;
uniform int seed4;
uniform int seed5;
uniform int seed6;
uniform int seed7;
uniform int seed8;

uniform int state3_1;
uniform int state4_1;
uniform int state5_2;
uniform int state6_2;
uniform int state7_3;
uniform int state8_2;

// 2. textures
// ---------------

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;
uniform sampler2D tex5;
uniform sampler2D tex6;
uniform sampler2D tex7;
uniform sampler2D tex8;
uniform sampler2D tex9;

// 3. definitions
// --------------

#define PI 3.1415927

// 4. functions
// ------------

// BASICS

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

// RANDOM

float rand(float seed){
    float v=pow(abs(seed),6./7.);
    v*=sin(v)+1.;
    return fract(v);
}

float rand(vec2 n){
    return rand(n.x * 1234 + n.y * 9876);
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

float mean(vec3 v)
{
    return v.x * 0.3333 + v.y * 0.3333 + v.z * 0.3333;
}

float mean(vec4 v)
{
    return v.x * 0.3333 + v.y * 0.3333 + v.z * 0.3333;
}

// TIME

float randTime(float seed){
    return rand(seed + mod(floor(iTime * iTempo / 240), 1000));
}

float divider(float x)
{
    if (x < 0.1) return 0;
    if (x < 0.2) return 0.125;
    if (x < 0.3) return 0.25;
    if (x < 0.4) return 0.5;
    if (x < 0.6) return 1;
    if (x < 0.8) return 2;
    return 4;
}

float modTime(float k, float k2)
{
    return mod(divider(k) * iTime * iTempo * k2 / 240, 1);
}

float modTime(float k)
{
    return modTime(k, 1.0);
}

float sinTime(float k)
{
    return sin(modTime(k, 0.5) * 2 * PI);
}

float cosTime(float k)
{
    return cos(modTime(k, 0.5) * 2 * PI);
}

// MAGIC

vec2 magic_f(vec2 F, vec3 B, float i)
{
    return vec2(
        mix(F.x, randTime(i + 1), min(1, B.z + iDemo)),
        mix(F.y, randTime(i + 2), min(1, B.z + iDemo))
    );
}

vec2 magic_f(float i)
{
    return magic_f(vec2(0), vec3(0, 0, 1), i);
}

vec3 magic_b(vec3 B, float i)
{
    return vec3(
        mix(B.x, step(0.2, randTime(i + 3)), min(1, B.z + iDemo)),
        mix(B.y, step(0.5, randTime(i + 4)), min(1, B.z + iDemo)),
        min(1, B.z + iDemo)
    );
}

vec3 magic_b(float i)
{
    return magic_b(vec3(0, 0, 1), i);
}

bool magic_trigger(vec3 B, float i)
{
    return magic_b(B, i).x > 0;
}

bool magic_trigger(float i)
{
    return magic_b(i).x > 0;
}

float magic(vec2 F, vec3 B, float i)
{
    vec2 f = magic_f(F, B, i);
    vec3 b = magic_b(B, i);

    return mix(0, f.x * mix(1 - modTime(f.y), cosTime(f.y) * 0.5 + 0.5, b.y), b.x);
}

float magic(float i)
{
    return magic(vec2(0), vec3(0, 0, 1), i);
}

float magic_reverse(vec2 F, vec3 B, float i)
{
    vec2 f = magic_f(F, B, i);
    vec3 b = magic_b(B, i);

    return mix(0, f.x * mix(1 - modTime(f.y), modTime(f.y), b.y), b.x);
}

float magic_reverse(float i)
{
    return magic_reverse(vec2(0), vec3(0, 0, 1), i);
}

// EFFECTS

mat2 rot(float angle){
    return mat2(
        cos(angle*2.*PI),-sin(angle*2.*PI),
        sin(angle*2.*PI),cos(angle*2.*PI)
    );
}

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
    int iTimeId = int(iTime * iTempo / 60);
    float iTimeV = iTime * iTempo / 60 - iTimeId;

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

// CP 437

const int cp437[] = {
    0x0000, 0x0000, 0x0000, 0x0000, // 0x00, NUL
    0x151E, 0x8A87, 0xE19D, 0x789B, // 0x01, SOH, SMILEY BLACK
    0xFBFE, 0xFDF7, 0xEF73, 0x7FEC, // 0x02, STX, SMILEY LIGHT
    0xFFF6, 0x7773, 0x08CE, 0x0013, // 0x03, ETX, HEART
    0xFEC8, 0x7310, 0x08CE, 0x0013, // 0x04, EOT, DIAMOND
    0xFCEC, 0x7131, 0xECEF, 0x3137, // 0x05, ENQ, CLUB
    0xEC88, 0x3100, 0xECEF, 0x3137, // 0x06, ACK, SPADE
    0xC800, 0x3100, 0x008C, 0x0013, // 0x07, BEL, DOT
    0x37FF, 0xCEFF, 0xFF73, 0xFFEC, // 0x08, BS, INVERT DOT
    0x26C0, 0x4630, 0x0C62, 0x0364, // 0x09, HT, CIRCLE
    0xD93F, 0xB9CF, 0xF39D, 0xFC9B, // 0x0A, LF, INVERT CIRCLE
    0xE000, 0xBFEF, 0xE333, 0x1333, // 0x0B, VT, MALE
    0x666C, 0x6663, 0x8E8C, 0x1713, // 0x0C, FF, FEMALE
    0xCCCC, 0x0FCF, 0x7FEC, 0x0000, // 0x0D, CR, NOTE
    0x6E6E, 0xCFCF, 0x3766, 0x06EC, // 0x0E, SO, DOUBLE NOTE
    0x7CA9, 0xE359, 0x9AC7, 0x953E, // 0x0F, SI, SUN
    0xFF71, 0x7100, 0x017F, 0x0001, // 0x10, DLE, RIGHT TRIANGLE
    0xFC00, 0x7774, 0x000C, 0x0477, // 0x11, DC1, LEFT TRIANGLE
    0x8EC8, 0x1731, 0x8CE8, 0x1371, // 0x12, DC2, ARROW UP DOWN
    0x6666, 0x6666, 0x0606, 0x0606, // 0x13, DC3, DOUBLE EXCLAMATION
    0xEBBE, 0xDDDF, 0x0888, 0x0DDD, // 0x14, DC4, PARAGRAPH
    0x6C6C, 0x31C7, 0xE3C6, 0x1313, // 0x15, NAK, ?
    0x0000, 0x0000, 0x0EEE, 0x0777, // 0x16, SYN, HALF SQUARE BOTTOM
    0x8EC8, 0x1731, 0xF8CE, 0xF137, // 0x17, ETB, ARROW UP DOWN UNDERLINED
    0x8EC8, 0x1731, 0x0888, 0x0111, // 0x18, CAN, ARROW UP
    0x8888, 0x1111, 0x08CE, 0x0137, // 0x19, EM, ARROW DOWN
    0xF080, 0x7310, 0x0080, 0x0013, // 0x1A, SUB, ARROW RIGHT
    0xF6C0, 0x7000, 0x00C6, 0x0000, // 0x1B, ESC, ARROW LEFT
    0x3300, 0x0000, 0x00F3, 0x0070, // 0x1C, FS, ?
    0xF640, 0xF620, 0x0046, 0x0026, // 0x1D, GS, ARROW LEFT RIGHT
    0xEC80, 0x7310, 0x00FF, 0x00FF, // 0x1E, RS, TRIANGLE UP
    0xEFF0, 0x7FF0, 0x008C, 0x0013, // 0x1F, US, TRIANGLE DOWN
    0x0000, 0x0000, 0x0000, 0x0000, // 0x20, SPACE
    0xCEEC, 0x0110, 0x0C0C, 0x0000, // 0x21, !
    0x0666, 0x0333, 0x0000, 0x0000, // 0x22, "
    0x6F66, 0x3733, 0x066F, 0x0337, // 0x23, #
    0xE3EC, 0x1030, 0x0CF0, 0x0013, // 0x24, $
    0x8330, 0x1360, 0x036C, 0x0660, // 0x25, %
    0xEC6C, 0x6131, 0x0E3B, 0x0633, // 0x26, &
    0x0366, 0x0000, 0x0000, 0x0000, // 0x27, '
    0x66C8, 0x0001, 0x08C6, 0x0100, // 0x28, (
    0x88C6, 0x1100, 0x06C8, 0x0001, // 0x29, )
    0xFC60, 0xF360, 0x006C, 0x0063, // 0x2A, *
    0xFCC0, 0x3000, 0x00CC, 0x0000, // 0x2B, +
    0x0000, 0x0000, 0x6CC0, 0x0000, // 0x2C, ,
    0xF000, 0x3000, 0x0000, 0x0000, // 0x2D, -
    0x0000, 0x0000, 0x0CC0, 0x0000, // 0x2E, .
    0xC800, 0x0136, 0x0136, 0x0000, // 0x2F, /
    0xB33E, 0x7763, 0x0E7F, 0x0366, // 0x30, 0
    0xCCEC, 0x0000, 0x0FCC, 0x0300, // 0x31, 1
    0xC03E, 0x1331, 0x0F36, 0x0330, // 0x32, 2
    0xC03E, 0x1331, 0x0E30, 0x0133, // 0x33, 3
    0x36C8, 0x3333, 0x080F, 0x0737, // 0x34, 4
    0x0F3F, 0x3103, 0x0E30, 0x0133, // 0x35, 5
    0xF36C, 0x1001, 0x0E33, 0x0133, // 0x36, 6
    0x803F, 0x1333, 0x0CCC, 0x0000, // 0x37, 7
    0xE33E, 0x1331, 0x0E33, 0x0133, // 0x38, 8
    0xE33E, 0x3331, 0x0E80, 0x0013, // 0x39, 9
    0x0CC0, 0x0000, 0x0CC0, 0x0000, // 0x3A, :
    0x0CC0, 0x0000, 0x6CC0, 0x0000, // 0x3B, ;
    0x36C8, 0x0001, 0x08C6, 0x0100, // 0x3C, <
    0x0F00, 0x0300, 0x00F0, 0x0030, // 0x3D, =
    0x08C6, 0x3100, 0x06C8, 0x0001, // 0x3E, >
    0x803E, 0x1331, 0x0C0C, 0x0000, // 0x3F, ?
    0xBB3E, 0x7763, 0x0E3B, 0x0107, // 0x40, @
    0x33EC, 0x3310, 0x033F, 0x0333, // 0x41, A
    0xE66F, 0x3663, 0x0F66, 0x0366, // 0x42, B
    0x336C, 0x0063, 0x0C63, 0x0360, // 0x43, C
    0x666F, 0x6631, 0x0F66, 0x0136, // 0x44, D
    0xE66F, 0x1147, 0x0F66, 0x0741, // 0x45, E
    0xE66F, 0x1147, 0x0F66, 0x0001, // 0x46, F
    0x336C, 0x0063, 0x0C63, 0x0767, // 0x47, G
    0xF333, 0x3333, 0x0333, 0x0333, // 0x48, H
    0xCCCE, 0x0001, 0x0ECC, 0x0100, // 0x49, I
    0x0008, 0x3337, 0x0E33, 0x0133, // 0x4A, J
    0xE667, 0x1366, 0x0766, 0x0663, // 0x4B, K
    0x666F, 0x0000, 0x0F66, 0x0764, // 0x4C, L
    0xFF73, 0x7776, 0x033B, 0x0666, // 0x4D, M
    0xBF73, 0x7666, 0x0333, 0x0667, // 0x4E, N
    0x336C, 0x6631, 0x0C63, 0x0136, // 0x4F, O
    0xE66F, 0x3663, 0x0F66, 0x0000, // 0x50, P
    0x333E, 0x3331, 0x08EB, 0x0313, // 0x51, Q
    0xE66F, 0x3663, 0x0766, 0x0663, // 0x52, R
    0xE73E, 0x0031, 0x0E38, 0x0133, // 0x53, S
    0xCCDF, 0x0023, 0x0ECC, 0x0100, // 0x54, T
    0x3333, 0x3333, 0x0F33, 0x0333, // 0x55, U
    0x3333, 0x3333, 0x0CE3, 0x0013, // 0x56, V
    0xB333, 0x6666, 0x037F, 0x0677, // 0x57, W
    0xC633, 0x1366, 0x036C, 0x0631, // 0x58, X
    0xE333, 0x1333, 0x0ECC, 0x0100, // 0x59, Y
    0x813F, 0x1367, 0x0F6C, 0x0764, // 0x5A, Z
    0x666E, 0x0001, 0x0E66, 0x0100, // 0x5B, [
    0x8C63, 0x1000, 0x0000, 0x0463, // 0x5C, 
    0x888E, 0x1111, 0x0E88, 0x0111, // 0x5D, ]
    0x36C8, 0x6310, 0x0000, 0x0000, // 0x5E, ^
    0x0000, 0x0000, 0xF000, 0xF000, // 0x5F, _
    0x08CC, 0x0100, 0x0000, 0x0000, // 0x60, `
    0x0E00, 0x3100, 0x0E3E, 0x0633, // 0x61, a
    0xE667, 0x3000, 0x0B66, 0x0366, // 0x62, b
    0x3E00, 0x3100, 0x0E33, 0x0130, // 0x63, c
    0xE008, 0x3333, 0x0E33, 0x0633, // 0x64, d
    0x3E00, 0x3100, 0x0E3F, 0x0103, // 0x65, e
    0xF66C, 0x0031, 0x0F66, 0x0000, // 0x66, f
    0x3E00, 0x3600, 0xF0E3, 0x1333, // 0x67, g
    0xE667, 0x6300, 0x0766, 0x0666, // 0x68, h
    0xCE0C, 0x0000, 0x0ECC, 0x0100, // 0x69, i
    0x0000, 0x3303, 0xE330, 0x1333, // 0x6A, j
    0x6667, 0x3600, 0x076E, 0x0631, // 0x6B, k
    0xCCCE, 0x0000, 0x0ECC, 0x0100, // 0x6C, l
    0xF300, 0x7300, 0x03BF, 0x0667, // 0x6D, m
    0x3F00, 0x3100, 0x0333, 0x0333, // 0x6E, n
    0x3E00, 0x3100, 0x0E33, 0x0133, // 0x6F, o
    0x6B00, 0x6300, 0xF6E6, 0x0036, // 0x70, p
    0x3E00, 0x3600, 0x80E3, 0x7333, // 0x71, q
    0xEB00, 0x6300, 0x0F66, 0x0006, // 0x72, r
    0x3E00, 0x0300, 0x0F0E, 0x0131, // 0x73, s
    0xCEC8, 0x0300, 0x08CC, 0x0120, // 0x74, t
    0x3300, 0x3300, 0x0E33, 0x0633, // 0x75, u
    0x3300, 0x3300, 0x0CE3, 0x0013, // 0x76, v
    0xB300, 0x6600, 0x06FF, 0x0377, // 0x77, w
    0x6300, 0x3600, 0x036C, 0x0631, // 0x78, x
    0x3300, 0x3300, 0xF0E3, 0x1333, // 0x79, y
    0x9F00, 0x1300, 0x0F6C, 0x0320, // 0x7A, z
    0x7CC8, 0x0003, 0x08CC, 0x0300, // 0x7B, {
    0x0888, 0x0111, 0x0888, 0x0111, // 0x7C, |
    0x8CC7, 0x3000, 0x07CC, 0x0000, // 0x7D, }
    0x00BE, 0x0036, 0x0000, 0x0000, // 0x7E, ~
    0x6C80, 0x3100, 0x0F33, 0x0766, // 0x7F, DEL, HOUSE
    0x333E, 0x3031, 0xE08E, 0x1311, // 0x80, Ç
    0x3030, 0x3030, 0x0E33, 0x0733, // 0x81, ü
    0x3E08, 0x3103, 0x0E3F, 0x0103, // 0x82, é
    0x0C3E, 0x63C7, 0x0C6C, 0x0F67, // 0x83, â
    0x0E03, 0x3103, 0x0E3E, 0x0733, // 0x84, ä
    0x0E07, 0x3100, 0x0E3E, 0x0733, // 0x85, à
    0x0ECC, 0x3100, 0x0E3E, 0x0733, // 0x86, å
    0x3E00, 0x0100, 0xC0E3, 0x1310, // 0x87, ç
    0x6C3E, 0x63C7, 0x0C6E, 0x0307, // 0x88, ê
    0x3E03, 0x3103, 0x0E3F, 0x0103, // 0x89, ë
    0x3E07, 0x3100, 0x0E3F, 0x0103, // 0x8A, è
    0xCE03, 0x0003, 0x0ECC, 0x0100, // 0x8B, ï
    0x8C3E, 0x1163, 0x0C88, 0x0311, // 0x8C, î
    0xCE07, 0x0000, 0x0ECC, 0x0100, // 0x8D, ì
    0x36C3, 0x6316, 0x033F, 0x0667, // 0x8E, Ä
    0xE0CC, 0x1000, 0x03F3, 0x0333, // 0x8F, Å
    0x6F08, 0x0303, 0x0F6E, 0x0301, // 0x90, É
    0x0E00, 0x3F00, 0x0E3E, 0x0F3F, // 0x91, æ
    0xF36C, 0x7337, 0x0333, 0x0733, // 0x92, Æ
    0xE03E, 0x1031, 0x0E33, 0x0133, // 0x93, ô
    0xE030, 0x1030, 0x0E33, 0x0133, // 0x94, ö
    0xE070, 0x1000, 0x0E33, 0x0133, // 0x95, ò
    0x303E, 0x3031, 0x0E33, 0x0733, // 0x96, û
    0x3070, 0x3000, 0x0E33, 0x0733, // 0x97, ù
    0x3030, 0x3030, 0xF0E3, 0x1333, // 0x98, ÿ
    0x6C83, 0x631C, 0x08C6, 0x0136, // 0x99, Ö
    0x3303, 0x3303, 0x0E33, 0x0133, // 0x9A, Ü
    0x3E88, 0x0711, 0x88E3, 0x1170, // 0x9B, ¢
    0xF66C, 0x0231, 0x0F76, 0x0360, // 0x9C, £
    0xFE33, 0x3133, 0xCCFC, 0x0030, // 0x9D, ¥
    0xF33F, 0x5331, 0x3333, 0xE6F6, // 0x9E, ₧
    0xC880, 0x31D7, 0xEB88, 0x0111, // 0x9F, ƒ
    0x0E08, 0x3103, 0x0E3E, 0x0733, // 0xA0, á
    0xCE0C, 0x0001, 0x0ECC, 0x0100, // 0xA1, í
    0xE080, 0x1030, 0x0E33, 0x0133, // 0xA2, ó
    0x3080, 0x3030, 0x0E33, 0x0733, // 0xA3, ú
    0xF0F0, 0x1010, 0x0333, 0x0333, // 0xA4, ñ
    0x730F, 0x3303, 0x03BF, 0x0333, // 0xA5, Ñ
    0xC66C, 0x7333, 0x00E0, 0x0070, // 0xA6, ª
    0xC66C, 0x1331, 0x00E0, 0x0030, // 0xA7, º
    0x6C0C, 0x0000, 0x0E33, 0x0130, // 0xA8, ¿
    0xF000, 0x3000, 0x0033, 0x0000, // 0xA9, ⌐
    0xF000, 0x3000, 0x0000, 0x0033, // 0xAA, ¬
    0xB333, 0x736C, 0x036C, 0xF36C, // 0xAB, ½
    0xB333, 0xD36C, 0x036C, 0xCFFE, // 0xAC, ¼
    0x8088, 0x1011, 0x0888, 0x0111, // 0xAD, ¡
    0x36C0, 0x36C0, 0x00C6, 0x00C6, // 0xAE, «
    0xC630, 0xC630, 0x0036, 0x0036, // 0xAF, »
    0x1414, 0x1414, 0x1414, 0x1414, // 0xB0, ░
    0x5A5A, 0x5A5A, 0x5A5A, 0x5A5A, // 0xB1, ▒
    0xBEBE, 0xBEBE, 0xBEBE, 0xBEBE, // 0xB2, ▓
    0x8888, 0x1111, 0x8888, 0x1111, // 0xB3, │
    0x8888, 0x1111, 0x888F, 0x1111, // 0xB4, ┤
    0x8F88, 0x1111, 0x888F, 0x1111, // 0xB5, ╡
    0xCCCC, 0x6666, 0xCCCF, 0x6666, // 0xB6, ╢
    0x0000, 0x0000, 0xCCCF, 0x6667, // 0xB7, ╖
    0x8F00, 0x1100, 0x888F, 0x1111, // 0xB8, ╕
    0x0FCC, 0x6666, 0xCCCF, 0x6666, // 0xB9, ╣
    0xCCCC, 0x6666, 0xCCCC, 0x6666, // 0xBA, ║
    0x0F00, 0x6700, 0xCCCF, 0x6666, // 0xBB, ╗
    0x0FCC, 0x6666, 0x000F, 0x0007, // 0xBC, ╝
    0xCCCC, 0x6666, 0x000F, 0x0007, // 0xBD, ╜
    0x8F88, 0x1111, 0x000F, 0x0001, // 0xBE, ╛
    0x0000, 0x0000, 0x888F, 0x1111, // 0xBF, ┐
    0x8888, 0x1111, 0x0008, 0x000F, // 0xC0, └
    0x8888, 0x1111, 0x000F, 0x000F, // 0xC1, ┴
    0x0000, 0x0000, 0x888F, 0x111F, // 0xC2, ┬
    0x8888, 0x1111, 0x8888, 0x111F, // 0xC3, ├
    0x0000, 0x0000, 0x000F, 0x000F, // 0xC4, ─
    0x8888, 0x1111, 0x888F, 0x111F, // 0xC5, ┼
    0x8888, 0x1F11, 0x8888, 0x111F, // 0xC6, ╞
    0xCCCC, 0x6666, 0xCCCC, 0x666E, // 0xC7, ╟
    0xCCCC, 0x0E66, 0x000C, 0x000F, // 0xC8, ╚
    0xCC00, 0x0F00, 0xCCCC, 0x666E, // 0xC9, ╔
    0x0FCC, 0x0E66, 0x000F, 0x000F, // 0xCA, ╩
    0x0F00, 0x0F00, 0xCCCF, 0x666E, // 0xCB, ╦
    0xCCCC, 0x0E66, 0xCCCC, 0x666E, // 0xCC, ╠
    0x0F00, 0x0F00, 0x000F, 0x000F, // 0xCD, ═
    0x0FCC, 0x0E66, 0xCCCF, 0x666E, // 0xCE, ╬
    0x0F88, 0x0F11, 0x000F, 0x000F, // 0xCF, ╧
    0xCCCC, 0x6666, 0x000F, 0x000F, // 0xD0, ╨
    0x0F00, 0x0F00, 0x888F, 0x111F, // 0xD1, ╤
    0x0000, 0x0000, 0xCCCF, 0x666F, // 0xD2, ╥
    0xCCCC, 0x6666, 0x000C, 0x000F, // 0xD3, ╙
    0x8888, 0x1F11, 0x0008, 0x000F, // 0xD4, ╘
    0x8800, 0x1F00, 0x8888, 0x111F, // 0xD5, ╒
    0x0000, 0x0000, 0xCCCC, 0x666F, // 0xD6, ╓
    0xCCCC, 0x6666, 0xCCCF, 0x666F, // 0xD7, ╫
    0x8F88, 0x1F11, 0x888F, 0x111F, // 0xD8, ╪
    0x8888, 0x1111, 0x000F, 0x0001, // 0xD9, ┘
    0x0000, 0x0000, 0x8888, 0x111F, // 0xDA, ┌
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, // 0xDB, █
    0x0000, 0x0000, 0xFFFF, 0xFFFF, // 0xDC, ▄
    0xFFFF, 0x0000, 0xFFFF, 0x0000, // 0xDD, ▌
    0x0000, 0xFFFF, 0x0000, 0xFFFF, // 0xDE, ▐
    0xFFFF, 0xFFFF, 0x0000, 0x0000, // 0xDF, ▀
    0xBE00, 0x3600, 0x0EB3, 0x0631, // 0xE0, α
    0xF3E0, 0x1310, 0x33F3, 0x0013, // 0xE1, ß
    0x33F0, 0x0330, 0x0333, 0x0000, // 0xE2, Γ
    0x66F0, 0x3370, 0x0666, 0x0333, // 0xE3, π
    0xC63F, 0x0033, 0x0F36, 0x0330, // 0xE4, Σ
    0xBE00, 0x1700, 0x0EBB, 0x0011, // 0xE5, σ
    0x6660, 0x6660, 0x36E6, 0x0036, // 0xE6, µ
    0x8BE0, 0x1360, 0x0888, 0x0111, // 0xE7, τ
    0x3ECF, 0x3103, 0xFCE3, 0x3013, // 0xE8, Φ
    0xF36C, 0x7631, 0x0C63, 0x0136, // 0xE9, Θ
    0x336C, 0x6631, 0x0766, 0x0733, // 0xEA, Ω
    0xE8C8, 0x3103, 0x0E33, 0x0133, // 0xEB, δ
    0xBE00, 0xD700, 0x00EB, 0x007D, // 0xEC, ∞
    0xBE00, 0xD736, 0x36EB, 0x007D, // 0xED, φ
    0xF36C, 0x1001, 0x0C63, 0x0100, // 0xEE, ε
    0x333E, 0x3331, 0x0333, 0x0333, // 0xEF, ∩
    0xF0F0, 0x3030, 0x00F0, 0x0030, // 0xF0, ≡
    0xCFCC, 0x0300, 0x0F0C, 0x0300, // 0xF1, ±
    0xC8C6, 0x0100, 0x0F06, 0x0300, // 0xF2, ≥
    0xC6C8, 0x0001, 0x0F08, 0x0301, // 0xF3, ≤
    0x8880, 0x1DD7, 0x8888, 0x1111, // 0xF4, ⌠
    0x8888, 0x1111, 0xEBB8, 0x0111, // 0xF5, ⌡
    0xF0CC, 0x3000, 0x0CC0, 0x0000, // 0xF6, ÷
    0x0BE0, 0x0360, 0x00BE, 0x0036, // 0xF7, ≈
    0xC66C, 0x1331, 0x0000, 0x0000, // 0xF8, °
    0x8000, 0x1000, 0x0008, 0x0001, // 0xF9, ∙
    0x0000, 0x0000, 0x0008, 0x0001, // 0xFA, ·
    0x0000, 0x333F, 0x8C67, 0x3333, // 0xFB, √
    0x666E, 0x3331, 0x0006, 0x0003, // 0xFC, ⁿ
    0x6C8E, 0x0010, 0x000E, 0x0001, // 0xFD, ²
    0xCC00, 0x3300, 0x00CC, 0x0033, // 0xFE, ■
    0x0000, 0x0000, 0x0000, 0x0000, // 0xFF, NBSP
};

const int charsets[] = {
    0x30, 10, // NUMBERS
    0x41, 26, // UPPERCASE LETTERS
    0x61, 26, // LOWERCASE LETTERS
    0x01, 15, // SYMBOLS
    0x18, 4, // ARROWS
    0xB3, 40, // BARS
    0xE0, 16, // GREEK
    0x80, 28, // ACCENTS
    0xB0, 3, // SHADES
};

#define CHARSETS 8

const int hex_chars[] = {
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,
    0x41,
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
};

bool char(vec2 pos, int code)
{
    if (pos.x < 0.0 || pos.y < 0.0 || pos.x >= 1.0 || pos.y >= 1.0) {
        return false;
    }
    ivec2 pos2 = ivec2(pos.x * 8.0, (1 - pos.y) * 8.0);
    ivec2 subpos = pos2 % 4;
    int v = int(pow(2, subpos.y * 4 + subpos.x));
    int d = int(pos2.y * 0.25) * 2 + int(pos2.x * 0.25);
    return (cp437[code * 4 + d] & v) > 0;
}

float char_at(vec2 uv, vec2 pos, int code)
{
    return char(uv - pos, code) ? 1 : 0;
}

float write_5(vec2 uv, vec2 pos, int str[5])
{
    float d = 0;
    int i;

    for (i = 0; i < 5; i++) {
        if (str[i] == 0) {
            return d;
        }

        d += char_at(uv, pos + vec2(i, 0), str[i]);
    }

    return d;
}

float write_10(vec2 uv, vec2 pos, int str[10])
{
    float d = 0;
    int i;

    for (i = 0; i < 10; i++) {
        if (str[i] == 0) {
            return d;
        }

        d += char_at(uv, pos + vec2(i, 0), str[i]);
    }

    return d;
}

float write_20(vec2 uv, vec2 pos, int str[20])
{
    float d = 0;
    int i;

    for (i = 0; i < 20; i++) {
        if (str[i] == 0) {
            return d;
        }

        d += char_at(uv, pos + vec2(i, 0), str[i]);
    }

    return d;
}

float write_int(vec2 uv, vec2 pos, uint value, uint magnitude)
{
    int i;
    uint m = 1;
    float d = 0;
    for (i = 0; i < magnitude; i++) {
        if (i == 0 || value >= m) {
            d += char_at(uv, pos + vec2(magnitude - i - 1, 0), int(0x30 + (value % (m * 10u)) / m));
            m *= 10u;
        }
    }
    return d;
}

int read(sampler2D tex, vec2 uv, float k, int d, float t)
{
    float inv_k = 1 / k;
    vec2 tex_uv = floor(uv * k) * inv_k;
    tex_uv += vec2(d % 2, floor(d * 0.5)) * 0.5 * inv_k;
    return
        ((mean(reframe(tex, tex_uv + vec2(0, 3) * inv_k * 0.125)) > t) ? 1 : 0) + 
        ((mean(reframe(tex, tex_uv + vec2(0, 2) * inv_k * 0.125)) > t) ? 2 : 0) +
        ((mean(reframe(tex, tex_uv + vec2(0, 1) * inv_k * 0.125)) > t) ? 4 : 0) +
        ((mean(reframe(tex, tex_uv + vec2(0, 0) * inv_k * 0.125)) > t) ? 8 : 0) +
        ((mean(reframe(tex, tex_uv + vec2(1, 3) * inv_k * 0.125)) > t) ? 16 : 0) +
        ((mean(reframe(tex, tex_uv + vec2(1, 2) * inv_k * 0.125)) > t) ? 32 : 0) +
        ((mean(reframe(tex, tex_uv + vec2(1, 1) * inv_k * 0.125)) > t) ? 64 : 0) +
        ((mean(reframe(tex, tex_uv + vec2(1, 0) * inv_k * 0.125)) > t) ? 128 : 0) +
        ((mean(reframe(tex, tex_uv + vec2(2, 3) * inv_k * 0.125)) > t) ? 256 : 0) +
        ((mean(reframe(tex, tex_uv + vec2(2, 2) * inv_k * 0.125)) > t) ? 512 : 0) +
        ((mean(reframe(tex, tex_uv + vec2(2, 1) * inv_k * 0.125)) > t) ? 1024 : 0) +
        ((mean(reframe(tex, tex_uv + vec2(2, 0) * inv_k * 0.125)) > t) ? 2048 : 0) +
        ((mean(reframe(tex, tex_uv + vec2(3, 3) * inv_k * 0.125)) > t) ? 4096 : 0) +
        ((mean(reframe(tex, tex_uv + vec2(3, 2) * inv_k * 0.125)) > t) ? 8192 : 0) +
        ((mean(reframe(tex, tex_uv + vec2(3, 1) * inv_k * 0.125)) > t) ? 16384 : 0) +
        ((mean(reframe(tex, tex_uv + vec2(3, 0) * inv_k * 0.125)) > t) ? 32768 : 0);
}

// https://web.archive.org/web/20151229003112/http://blogs.msdn.com/b/jeuge/archive/2005/06/08/hakmem-bit-count.aspx
int bit_count(int u)                         
{
        int c;
        c = u - ((u >> 1) & 033333333333) - ((u >> 2) & 011111111111);
        return ((c + (c >> 3)) & 030707070707) % 63;
}

int guess_char(sampler2D tex, vec2 uv, float k, float t)
{
    int b0 = read(tex, uv, k, 0, t);
    int b1 = read(tex, uv, k, 1, t);
    int b2 = read(tex, uv, k, 2, t);
    int b3 = read(tex, uv, k, 3, t);
    
    int mc = 0;
    int mb = 100;
    int i;
    int b;

    for (i = 0x01; i <= 0xFF; i++) {
        if (i == 0x20 || i == 0xff || i == 0xDB) {
            continue;
        }
        b = bit_count(cp437[i * 4] ^ b0) + bit_count(cp437[i * 4 + 1] ^ b1) + bit_count(cp437[i * 4 + 2] ^ b2) +  + bit_count(cp437[i * 4 + 3] ^ b3);
        if (b < mb) {
            mb = b;
            mc = i;
        }
    }

    return mc;
}

// 5. generators
// -------------

subroutine vec4 src_stage_sub(vec2 vUV, int seed);

subroutine uniform src_stage_sub src_stage;

vec4 src_thru(vec2 vUV, sampler2D tex, int seed)
{
    // start

	vec2 uv0 = vUV.st;

    // controls

    float hue = magic(seed + 10);
    float saturation = magic(seed + 20);
    float light = magic(seed + 30);

    // logic

    vec3 c = texture(tex, uv0).xyz;

    c = shift3(c, hue);

    c *= 1 + saturation;
    c = mix(c + light * 2.0, c - (1 - light) * 2.0, step(0.5, light));

    // output

   return vec4(c, 1.);
}

// SRC 1: feedback + thru
subroutine(src_stage_sub) vec4 src_1(vec2 vUV, int seed)
{
    return src_thru(vUV, tex0, seed);
}

// SRC 2 : lines
subroutine(src_stage_sub) vec4 src_2(vec2 vUV, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float thickness = magic(seed + 10);
    float rotation = magic(seed + 20);
    float distort = magic(seed + 30);

    // logic

    vec2 uv2 = uv1;
    uv2.y *= cos(uv2.x * 5 * distort);
    uv2 *= rot(rotation + iTime * iTempo / 960);
    float k = thickness * 2;
    uv2.y = cmod(uv2.y, k * 2 + 0.1);
    float f = step(uv2.y, k * 0.125 + 0.05) * step(-uv2.y, k * 0.125 + 0.01);
    
    return vec4(f);
}

// SRC 3 : dots
subroutine(src_stage_sub) vec4 src_3(vec2 vUV, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float zoom = magic(seed + 10);
    float rotation = magic(seed + 20);
    float lens_v = magic(seed + 30);

    // logic

    vec2 uv2 = uv1;
    float k1 = lens_v * 5;
    uv2 = lens(uv2, -k1, k1);
    uv2 = kal(uv2, 5);
    uv2 *= rot(rotation + iTime * iTempo / 960);
    float k = zoom * 0.1 + 0.05;
    uv2 = cmod(uv2, k * 2);
    float f = step(length(uv2), k / (1 + length(uv1) * 2));
    
    return vec4(f);
}

// SRC 4 : waves
subroutine(src_stage_sub) vec4 src_4(vec2 vUV, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float spacing = magic(seed + 10);
    float thickness = magic(seed + 20);
    float scroll = magic_reverse(seed + 30);

    // logic

    vec2 uv2 = uv1;
    uv2.y += 0.5;
    uv2 *= 2.25;
    uv2 = vec2((uv2.x + 1) * 0.5, -uv2.y);
    float m1 = spacing * 4.5 + 0.5;
    float y = log(-uv2.y) * m1;
    y = mod(y + scroll * 5.0 - iTime * iTempo / 960, 5.);
    float id = floor(y) * 32;
    float s = cos(uv2.x * rand(id + 837) * 100 + rand(id + 281) * PI)
                + cos(uv2.x * rand(id + 231) * 100 + rand(id + 526) * PI)
                + cos(uv2.x * rand(id + 746) * 100 + rand(id + 621) * PI)
                + cos(uv2.x * rand(id + 235) * 100 + rand(id + 315) * PI)
                + cos(uv2.x * rand(id + 782) * 100 + rand(id + 314) * PI)
                + cos(uv2.x * rand(id + 241) * 100 + rand(id + 734) * PI)
                + cos(uv2.x * rand(id + 416) * 100 + rand(id + 425) * PI)
                + cos(uv2.x * rand(id + 315) * 100 + rand(id + 525) * PI)
                + cos(uv2.x * rand(id + 423) * 100 + rand(id + 743) * PI)
                + cos(uv2.x * rand(id + 637) * 100 + rand(id + 245) * PI);
    s *= 0.1;
    float cut =  0.025 + thickness * 0.475;
    float y2 = min(1.0, -(uv2.y));
    float f = (0.1 + 0.9 * (cos((y2 + 1.0) * PI) * 0.5 + 0.5)) * step(uv2.y, 0.) * step(fract(y + (s - 1) * (1 - cut) * 0.5), cut);//step(uv2.y, 0.) * mod(-uv2.y * 1.0, 1.0);
    
    return vec4(f);
}

// SRC 5 : noise
subroutine(src_stage_sub) vec4 src_5(vec2 vUV, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float zoom = magic(seed + 10);
    float voronoi_distort = magic(seed + 20);
    float details = magic(seed + 30);
    float noise_factor = magic(seed + 40);

    // logic

    vec2 uv2 = uv1;
    uv2 *= zoom * 20 + 3;
    uv2.x += iTime * iTempo / 60;
    vec4 data = voronoi(uv2, voronoi_distort);
    float f = data.x / (data.x + data.y);
    f = sin(f * PI * (details * 20)) * 0.5 + 1;
    int nf = int(noise_factor * 6);
    f *= mix(1, noise_f(uv2, nf - 1), step(0.0, float(nf)));
    
    return vec4(f);
}

// SRC 6 : video in 1 + thru
subroutine(src_stage_sub) vec4 src_6(vec2 vUV, int seed)
{
    return src_thru(vUV, tex3, seed);
}

// SRC 7 : cp437
subroutine(src_stage_sub) vec4 src_7(vec2 vUV, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float zoom = magic(seed + 10);
    vec2 charset = magic_f(seed + 20);
    vec3 charset_ctrl = magic_b(seed + 20);
    float char_delta = magic(seed + 30);

    // logic

    vec2 uv2 = uv1;
    uv2 *= zoom * 20 + 3;
    uv2 += iTime * iTempo / 60;
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
#define SENTENCE_COUNT 10

const int sentences[SENTENCE_COUNT][20] = {
    {98, 101, 76, 111, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {98, 101, 76, 111, 119, 32, 98, 101, 76, 111, 119, 32, 98, 101, 76, 111, 119, 0, 0, 0},
    {67, 39, 101, 115, 116, 32, 108, 97, 32, 116, 101, 117, 102, 0, 0, 0, 0, 0, 0, 0},
    {76, 105, 108, 108, 101, 32, 86, 74, 32, 70, 101, 115, 116, 0, 0, 0, 0, 0, 0, 0},
    {80, 111, 117, 114, 32, 108, 101, 115, 32, 121, 101, 117, 120, 0, 0, 0, 0, 0, 0, 0},
    {80, 111, 117, 114, 32, 108, 101, 115, 32, 111, 114, 101, 105, 108, 108, 101, 115, 0, 0, 0},
    {77, 97, 99, 104, 105, 110, 101, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {83, 97, 117, 118, 97, 103, 101, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {68, 114, 117, 109, 32, 38, 32, 68, 114, 117, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {86, 56, 32, 43, 32, 83, 104, 97, 100, 101, 114, 115, 32, 43, 32, 67, 97, 109, 46, 0},
};

const int lengths[SENTENCE_COUNT] = {
    5, 17, 13, 13, 13, 17, 8, 8, 11, 19
};

subroutine(src_stage_sub) vec4 src_8(vec2 vUV, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float zoom = magic(seed + 10);
    float sentence = magic_reverse(seed + 20);
    float h_delta = magic(seed + 30);
    vec3 h_delta_b = magic_b(seed + 30);

    // logic

    vec2 uv2 = uv1;
    uv2 *= (1 + zoom) * 12;
    int s = int(sentence * (SENTENCE_COUNT - 1));
    uv2.x += floor(uv2.y) * (h_delta - 0.5) * 2;
    uv2.y = mix(uv2.y, mod(uv2.y, 1), h_delta_b.x);
    float f = write_20(uv2, vec2(-float(lengths[s]) * 0.5, 0), sentences[s]);
    
    return vec4(f);
}

// TODO SRC 9
subroutine(src_stage_sub) vec4 src_9(vec2 vUV, int seed)
{
    // start

    vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    // logic
    
    return texture(tex0, vUV);
}

// TODO SRC 10
subroutine(src_stage_sub) vec4 src_10(vec2 vUV, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    // logic
    
    return texture(tex0, vUV);
}

// SRC 11 : video in 2 + thru
subroutine(src_stage_sub) vec4 src_11(vec2 vUV, int seed)
{
    return src_thru(vUV, tex4, seed);
}

// TODO SRC 12
subroutine(src_stage_sub) vec4 src_12(vec2 vUV, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    // logic
    
    return texture(tex0, vUV);
}

// TODO SRC 13
subroutine(src_stage_sub) vec4 src_13(vec2 vUV, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    // logic
    
    return texture(tex0, vUV);
}

// TODO SRC 14
subroutine(src_stage_sub) vec4 src_14(vec2 vUV, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    // logic
    
    return texture(tex0, vUV);
}

// TODO SRC 15
subroutine(src_stage_sub) vec4 src_15(vec2 vUV, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    // logic
    
    return texture(tex0, vUV);
}

// SRC 16 : debug
subroutine(src_stage_sub) vec4 src_16(vec2 vUV, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // inputs

    int selected = 0; // TODO debug selected
    int page = 0; // TODO debug page
    int selected_src = 0; // TODO debug selected
    int selected_fx = 0; // TODO debug selected
    int selected_srca = state3_1;
    int selected_srcb = state4_1;
    int selected_fxa = state5_2;
    int selected_fxb = state6_2;
    int selected_mfx = state8_2;
    float fxa_value = magic(seed5);
    float fxb_value = magic(seed6);
    float mfx_value = magic(seed8);
    float mix_value = magic(seed7);
    int mix_type = state7_3 % 2;

    // logic

    const int texts[5][5] = {
        {0x46, 0x50, 0x53, 0x00, 0x00}, // FPS
        {0x54, 0x45, 0x4D, 0x50, 0x4F}, // TEMPO
        {0x54, 0x49, 0x4D, 0x45, 0x00}, // TIME
        {0x44, 0x45, 0x4D, 0x4F, 0x00}, // DEMO
        {0x4C, 0x49, 0x56, 0x45, 0x00}, // LIVE
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
        rect(uv2, vec2(7.5, 0), vec2(1.5, 0.1)) +
        h_rect(uv2, vec2(-9, -3.9), vec2(1), 0.1);

    // show selected src/fx
    f += char_at(uv2, vec2(-5.4, 1.45), hex_chars[selected_srca]);
    f += char_at(uv2, vec2(-5.4, -2.55), hex_chars[selected_srcb]);
    f += char_at(uv2, vec2(-2.4, 1.45), hex_chars[selected_fxa]);
    f += char_at(uv2, vec2(4.6, -0.55), hex_chars[selected_fxb]);
    f += char_at(uv2, vec2(-2.4, -2.55), hex_chars[selected_mfx]);

    // show current selected
    f += selected == 0 ? h_rect(uv2, vec2(-5, 2), vec2(1.2), 0.1) : 0;
    f += selected == 1 ? h_rect(uv2, vec2(-5, -2), vec2(1.2), 0.1) : 0;
    f += selected == 2 ? h_rect(uv2, vec2(-2, 2), vec2(1.2), 0.1) : 0;
    f += selected == 3 ? h_rect(uv2, vec2(5, 0), vec2(1.2), 0.1) : 0;
    f += selected == 4 ? h_rect(uv2, vec2(-2, -2), vec2(1.2), 0.1) : 0;

    // show selected src/fx
    f += selected_src == 0 ? h_rect(uv2, vec2(-5, 0.8), vec2(1, 0), 0.1) : 0;
    f += selected_src == 1 ? h_rect(uv2, vec2(-5, -3.2), vec2(1, 0), 0.1) : 0;
    f += selected_fx == 2 ? h_rect(uv2, vec2(-2, 0.8), vec2(1.2, 0), 0.1) : 0;
    f += selected_fx == 3 ? h_rect(uv2, vec2(5, -1.2), vec2(1, 0), 0.1) : 0;
    f += selected_fx == 4 ? h_rect(uv2, vec2(-2, -3.2), vec2(1, 0), 0.1) : 0;

    // show inputs / feedback
    f += selected_srca == 5 ? rect(uv2, vec2(-8, 2), vec2(2, 0.1)) : 0;
    f += selected_srca == 10 ? rect(uv2, vec2(-7, 2), vec2(1, 0.1)) + rect(uv2, vec2(-8, 0.5), vec2(0.1, 1.6)) + rect(uv2, vec2(-9, -1), vec2(1, 0.1)) : 0;
    f += (selected_srca == 0 || selected_srca % 5 != 0 && selected_srca >= 8) ? rect(uv2, vec2(-6.5, 2), vec2(0.5, 0.1)) + rect(uv2, vec2(0, 4), vec2(7, 0.1)) + rect(uv2, vec2(-7, 3), vec2(0.1, 1.1)) + rect(uv2, vec2(7, 2), vec2(0.1, 2.1)) : 0;
    f += selected_srcb == 5 ? rect(uv2, vec2(-6.5, -2), vec2(0.5, 0.1)) + rect(uv2, vec2(-7, -0.5), vec2(0.1, 1.6)) + rect(uv2, vec2(-8.5, 1), vec2(1.5, 0.1)) : 0;
    f += selected_srcb == 10 ? rect(uv2, vec2(-8, -2), vec2(2, 0.1)) : 0;
    f += (selected_srcb == 0 || selected_srcb % 5 != 0 && selected_srcb >= 8) ? rect(uv2, vec2(-6.5, -2), vec2(0.5, 0.1)) + rect(uv2, vec2(0, -4), vec2(7, 0.1)) + rect(uv2, vec2(-7, -3), vec2(0.1, 1.1)) + rect(uv2, vec2(7, -2), vec2(0.1, 2.1)) : 0;

    // show page
    f += char_at(uv2, vec2(-9.2, -4.3), hex_chars[page]);

    // show fx values
    f = mix(f, 1 - f, rect(uv2, vec2(-2, 1.1 + 0.9 * fxa_value), vec2(0.9, 0.9 * fxa_value)));
    f = mix(f, 1 - f, rect(uv2, vec2(5, -0.9 + 0.9 * fxb_value), vec2(0.9, 0.9 * fxb_value)));
    f = mix(f, 1 - f, rect(uv2, vec2(-2, -2.9 + 0.9 * mfx_value), vec2(0.9, 0.9 * mfx_value)));

    // show mix
    f += char_at(uv2, vec2(1.55, -0.6), mix_type > 0 ? 0x4B : 0x4D);
    f = mix(f, 1 - f, rect(uv2, vec2(2, -0.9 + 0.9 * mix_value), vec2(0.9, 0.9 * mix_value)));

    // show debug info
    float v = 0;
    float x = 0;

    x = -15;
    f += write_5(uv3, vec2(x,13), texts[0]);
    f += write_int(uv3, vec2(x - 4,13), iFPS, 3);
    v = min(1, iFPS/60.0);
    f += h_rect(uv3, vec2(x, 12), vec2(4, 0.5), 0.2);
    f += rect(uv3, vec2(x + 4 * v - 4, 12), vec2(4 * v, 0.4));
    
    x = 0;
    f += write_5(uv3, vec2(x,13), texts[1]);
    f += write_int(uv3, vec2(x - 4,13), int(iTempo), 3);
    v = modTime(1);
    f += h_rect(uv3, vec2(x, 12), vec2(4, 0.5), 0.2);
    f += rect(uv3, vec2(x + 4 * v - 4, 12), vec2(4 * v, 0.4));

    x = 15;
    f += write_5(uv3, vec2(x,13), texts[2]);
    f += write_int(uv3, vec2(x - 6,13), int(iTime), 5);
    v = fract(iTime);
    f += h_rect(uv3, vec2(x, 12), vec2(4, 0.5), 0.2);
    f += rect(uv3, vec2(x + 4 * v - 4, 12), vec2(4 * v, 0.4));

    f += write_5(uv3, vec2(-2,-15), iDemo > 0 ? texts[3] : texts[4]);

    return vec4(f);
}

// 6. effects
// ----------

subroutine vec4 fx_stage_sub(vec2 vUV, sampler2D previous, sampler2D feedback, int seed);

subroutine uniform fx_stage_sub fx_stage;

// FX 1 : thru
subroutine(fx_stage_sub) vec4 fx_1(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    // start

	vec2 uv0 = vUV.st;

    // controls

    float fx = magic(seed);

    float hue = magic(seed + 10);
    float saturation = magic(seed + 20);
    float light = magic(seed + 30);

    // logic

    vec3 c0 = texture(previous, uv0).xyz;
    vec3 c = c0;
    c = shift3(c, hue);
    c *= 1 + saturation;
    c = mix(c + light * 2.0, c - (1 - light) * 2.0, step(0.5, light));

    return vec4(mix(c0, c, fx), 1.0);
}

// FX 2 : feedback + shift
vec4 fx_shift(vec2 vUV, sampler2D src0, sampler2D src1, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float fx = magic(seed);

    float zoom = magic(seed + 10);
    float x_shift = magic(seed + 20);
    float y_shift = magic(seed + 30);

    // logic
    
    vec3 c0 = texture(src0, uv0).xyz;
    
    vec2 uv2 = uv1;
    uv2 = mix(uv2 * (1 + zoom * 2), uv2 * (zoom), step(0.5, zoom));
    uv2 += vec2(x_shift * ratio, y_shift) * 2;
    vec3 c = reframe(src1, uv2).xyz;

    return vec4(mix(c0, c, fx), 1.0);
}

subroutine(fx_stage_sub) vec4 fx_2(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    return fx_shift(vUV, previous, feedback, seed);
}

// FX 3 : shift
subroutine(fx_stage_sub) vec4 fx_3(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    return fx_shift(vUV, previous, previous, seed);
}

// FX 4 : colorize
subroutine(fx_stage_sub) vec4 fx_4(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    // start

	vec2 uv0 = vUV.st;

    // controls

    float fx = magic(seed);

    float c_black = magic(seed + 10);
    bool c_black_trigger = magic_trigger(seed + 10);
    float c_white = magic(seed + 20);
    bool c_white_trigger = magic_trigger(seed + 20);
    float delta = magic(seed + 30);

    // logic

    vec3 c0 = texture(previous, uv0).xyz;

    float f = mean(c0);
    float c_mix = mix(c_black, c_white, f) + delta;
    vec3 c = mix(c_black_trigger ? col(c_mix) : vec3(0), c_white_trigger ? col(c_mix) : vec3(1), f);

    return vec4(mix(c0, c, fx), 1.0);
}

// FX 5 : quantize
subroutine(fx_stage_sub) vec4 fx_5(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float fx = magic(seed);

    float pixel_size = magic(seed + 10);
    float quantize = magic(seed + 20);
    bool quantize_trigger = magic_trigger(seed + 20);
    float blur = magic(seed + 30);

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

    return vec4(mix(c0, c, fx), 1.0);
}

// FX 6 : dithering
subroutine(fx_stage_sub) vec4 fx_6(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float fx = magic(seed);

    float pixel_size = magic(seed + 10);
    bool pixel_size_trigger = magic_trigger(seed + 10);
    float quantize = magic(seed + 20);
    bool quantize_trigger = magic_trigger(seed + 20);
    float blur = magic(seed + 30);

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

    return vec4(mix(c0, c, fx), 1.0);
}

// FX 7 : tv
subroutine(fx_stage_sub) vec4 fx_7(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float fx = magic(seed);

    float lens_v = magic(seed + 10);
    float horizontal_noise = magic(seed + 20);
    float zoom = magic(seed + 30);

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

    return vec4(mix(c0, c, fx), 1.0);
}

// FX 8 : kaleidoscope
subroutine(fx_stage_sub) vec4 fx_8(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float fx = magic(seed);

    float axes = magic(seed + 10);
    float axes_trigger = magic_b(seed + 10).x;
    float rotation = magic(seed + 20);
    float h_scroll = magic(seed + 30);

    // logic

    vec3 c0 = texture(previous, uv0).xyz;
    
    vec2 uv2 = uv1;
    uv2 = mix(uv2, kal2(uv2 * rot(0.25), floor(axes * 9 + 1)) * vec2(1, -2) + vec2(0, -0.5), axes_trigger);    
    uv2 *= rot(rotation);
    uv2.x = (saw(uv2.x / ratio + 0.5 + h_scroll * 2) - 0.5) * ratio;
    vec3 c = reframe(previous, uv2).xyz;

    return vec4(mix(c0, c, fx), 1.0);
}

// FX 9 : cp437
subroutine(fx_stage_sub) vec4 fx_9(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float fx = magic(seed);

    float zoom = magic(seed + 10);
    vec2 charset = magic_f(seed + 20);
    vec3 charset_ctrl = magic_b(seed + 20);
    float char_delta = magic(seed + 30);
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

    return vec4(mix(c0, c, fx), 1.0);
}

// FX 10 : lens
subroutine(fx_stage_sub) vec4 fx_10(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float fx = magic(seed);

    float lens_v1 = magic(seed + 10);
    float lens_v2 = magic(seed + 20);
    float zoom = magic(seed + 30);
    float k = magic(seed + 40);

    // logic

    vec3 c0 = texture(previous, uv0).xyz;
    
    vec2 uv2 = uv1;
    uv2 *= 1 + zoom * 2;
    uv2 = lens(uv2, -lens_v2 * 10, lens_v1 * 10);
    vec3 c = reframe(previous, uv2).xyz;

    return vec4(mix(c0, c, fx), 1.0);
}

// TODO FX 11
subroutine(fx_stage_sub) vec4 fx_11(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float fx = magic(seed);

    // logic
    
    vec3 c0 = texture(previous, uv0).xyz;
    vec3 c = c0;

    return vec4(mix(c0, c, fx), 1.0);
}

// TODO FX 12
subroutine(fx_stage_sub) vec4 fx_12(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float fx = magic(seed);

    // logic
    
    vec3 c0 = texture(previous, uv0).xyz;
    vec3 c = c0;

    return vec4(mix(c0, c, fx), 1.0);
}

// TODO FX 13
subroutine(fx_stage_sub) vec4 fx_13(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float fx = magic(seed);

    // logic
    
    vec3 c0 = texture(previous, uv0).xyz;
    vec3 c = c0;

    return vec4(mix(c0, c, fx), 1.0);
}

// TODO FX 14
subroutine(fx_stage_sub) vec4 fx_14(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float fx = magic(seed);

    // logic

    vec3 c0 = texture(previous, uv0).xyz;
    vec3 c = c0;

    return vec4(mix(c0, c, fx), 1.0);
}

// TODO FX 15
subroutine(fx_stage_sub) vec4 fx_15(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float fx = magic(seed);

    // logic
    
    vec3 c0 = texture(previous, uv0).xyz;
    vec3 c = c0;

    return vec4(mix(c0, c, fx), 1.0);
}

// TODO FX 16
subroutine(fx_stage_sub) vec4 fx_16(vec2 vUV, sampler2D previous, sampler2D feedback, int seed)
{
    // start

	vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // controls

    float fx = magic(seed);

    // logic

    vec3 c0 = texture(previous, uv0).xyz;
    vec3 c = c0;

    return vec4(mix(c0, c, fx), 1.0);
}

// 7. mix
// ----------

subroutine vec4 mix_stage_sub(vec2 vUV, sampler2D tex_a, sampler2D tex_a, int seed);

subroutine uniform mix_stage_sub mix_stage;

// MIX 1 : mix
subroutine(mix_stage_sub) vec4 mix_1(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    // start

	vec2 uv0 = vUV.st;

    // controls

    float mix_src = magic(seed);

    // logic

    vec4 color_a = texture(ta, vUV);
    vec4 color_b = texture(tb, vUV);

    return mix(color_b, color_a, mix_src);
}

// MIX 2 : luminance key
subroutine(mix_stage_sub) vec4 mix_2(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    // start

	vec2 uv0 = vUV.st;

    // controls

    float mix_src = magic(seed);

    // logic
    
    vec4 color_a = texture(ta, vUV);
    vec4 color_b = texture(tb, vUV);

    float k = mean(color_a);

    return mix(color_b, color_a, step(mix_src, k));
}

// alternate mix 1 / 2 for random selection

subroutine(mix_stage_sub) vec4 mix_3(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    return mix_1(vUV, ta, tb, seed);
}

subroutine(mix_stage_sub) vec4 mix_4(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    return mix_2(vUV, ta, tb, seed);
}

subroutine(mix_stage_sub) vec4 mix_5(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    return mix_1(vUV, ta, tb, seed);
}

subroutine(mix_stage_sub) vec4 mix_6(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    return mix_2(vUV, ta, tb, seed);
}

subroutine(mix_stage_sub) vec4 mix_7(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    return mix_1(vUV, ta, tb, seed);
}

subroutine(mix_stage_sub) vec4 mix_8(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    return mix_2(vUV, ta, tb, seed);
}

subroutine(mix_stage_sub) vec4 mix_9(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    return mix_1(vUV, ta, tb, seed);
}

subroutine(mix_stage_sub) vec4 mix_10(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    return mix_2(vUV, ta, tb, seed);
}

subroutine(mix_stage_sub) vec4 mix_11(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    return mix_1(vUV, ta, tb, seed);
}

subroutine(mix_stage_sub) vec4 mix_12(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    return mix_2(vUV, ta, tb, seed);
}

subroutine(mix_stage_sub) vec4 mix_13(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    return mix_1(vUV, ta, tb, seed);
}

subroutine(mix_stage_sub) vec4 mix_14(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    return mix_2(vUV, ta, tb, seed);
}

subroutine(mix_stage_sub) vec4 mix_15(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    return mix_1(vUV, ta, tb, seed);
}

subroutine(mix_stage_sub) vec4 mix_16(vec2 vUV, sampler2D ta, sampler2D tb, int seed)
{
    return mix_2(vUV, ta, tb, seed);
}

const mat3x3 yuv_to_rgb = {{1,1,1},{0,-0.39465,2.03211},{1.13983,-0.5806,0}};

const float YUYV_FOURCC = 1448695129.0;

vec4 yuyvTex(sampler2D tex, vec2 vUV, int base_width) {
    float w = base_width - 1;
    
    int x = int(vUV.x * w);

    int xU = x - x % 2;
    int xV = x - x % 2 + 1;

    vec4 tU = texture(tex, vec2(xU / w, 1 - vUV.y));
    vec4 tV = texture(tex, vec2(xV / w, 1 - vUV.y));

    vec3 yuv = vec3(
        x % 2 == 0 ? tU.x : tV.x,
        tU.y - 0.5,
        tV.y - 0.5
    );

    return vec4(yuv_to_rgb * yuv, 1.0);
}