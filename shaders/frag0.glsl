#version 460

// COMMON DEFINITIONS AND FUNCTIONS
// ================================

// 1. real time uniforms
// ---------------------

uniform float iTime;
uniform float iTempo;
uniform vec2 iResolution;

// 2. other frames
// ---------------

uniform sampler2D frame0;
uniform sampler2D frame1;
uniform sampler2D frame2;
uniform sampler2D frame3;
uniform sampler2D frame4;
uniform sampler2D frame5;
uniform sampler2D frame6;
uniform sampler2D frame7;

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

// TIME

float randTime(float seed){
    return rand(seed + mod(floor(iTime * iTempo / 240), 1000));
}

float divider(float k, int m)
{
    // 2 -> 0, 0.5, 1, 2 | 3 -> 0, 0.25, 0.5, 1, 2, 4
    return k * (m * 2 - 1) < 1 ? 0 : pow(2, floor(k * (m * 2 - 1)) - m);
}

float modTime(float k, int m, float k2)
{
    return k * (m * 2  - 1) < 1 ? 0 : mod(divider(k, m) * iTime * iTempo * k2 / 120, 1);
}

float modTime(float k, int m)
{
    return modTime(k, m, 1.0);
}

float modTime(float k)
{
    return modTime(k, 2);
}

float sinTime(float k, int m)
{
    return sin(modTime(k, m, 0.5) * 2 * PI);
}

float sinTime(float k)
{
    return sinTime(k, 2);
}

float cosTime(float k, int m)
{
    return cos(modTime(k, m, 0.5) * 2 * PI);
}

float cosTime(float k)
{
    return cosTime(k, 2);
}

// MAGIC

vec2 magic_f(vec2 F, vec3 B, float i)
{
    return vec2(
        mix(F.x, randTime(i + 1), B.z),
        mix(F.y, randTime(i + 2), B.z)
    );
}

vec3 magic_b(vec3 B, float i)
{
    return vec3(
        mix(B.x, step(0.2, randTime(i + 3)), B.z),
        mix(B.y, step(0.5, randTime(i + 4)), B.z),
        B.z
    );
}

bool magic_trigger(vec3 B, float i)
{
    return magic_b(B, i).x > 0;
}

float magic(vec2 F, vec3 B, float i, int m)
{
    vec2 f = magic_f(F, B, i);
    vec3 b = magic_b(B, i);

    return mix(0, f.x * mix(1 - modTime(f.y, m), cosTime(f.y, m) * 0.5 + 0.5, b.y), b.x);
}

float magic_reverse(vec2 F, vec3 B, float i, int m)
{
    vec2 f = magic_f(F, B, i);
    vec3 b = magic_b(B, i);

    return mix(0, f.x * mix(1 - modTime(f.y, m), modTime(f.y, m), b.y), b.x);
}

float magic(vec2 F, vec3 B, float i)
{
    return magic(F, B, i, 4);
}

float magic(float i)
{
    return magic(vec2(0), vec3(0, 0, 1), i);
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

vec3 reframe(sampler2D frame, vec2 uv)
{
    uv = uv * vec2(iResolution.y / iResolution.x, 1)  + .5;
    uv = saw(uv);
    return texture(frame, uv).xyz;
}

vec3 reframe_b(sampler2D frame, vec2 uv)
{
    uv = uv * vec2(iResolution.y / iResolution.x, 1)  + .5;
    return texture(frame, uv).xyz;
}

// BLUR

vec3 gauss(sampler2D frame, vec2 uv, float f)
{
    return texture(frame, uv + vec2(f, 0)).xyz * 0.125
        +  texture(frame, uv + vec2(f, f)).xyz * 0.125
        +  texture(frame, uv + vec2(0, f)).xyz * 0.125
        +  texture(frame, uv + vec2(-f, f)).xyz * 0.125
        +  texture(frame, uv + vec2(-f, 0)).xyz * 0.125
        +  texture(frame, uv + vec2(-f, -f)).xyz * 0.125
        +  texture(frame, uv + vec2(0, -f)).xyz * 0.125
        +  texture(frame, uv + vec2(f, -f)).xyz * 0.125;
}

vec3 gauss2(sampler2D frame, vec2 uv, float f)
{
    return gauss(frame, uv + vec2(f, 0), f).xyz * 0.125
        +  gauss(frame, uv + vec2(f, f), f).xyz * 0.125
        +  gauss(frame, uv + vec2(0, f), f).xyz * 0.125
        +  gauss(frame, uv + vec2(-f, f), f).xyz * 0.125
        +  gauss(frame, uv + vec2(-f, 0), f).xyz * 0.125
        +  gauss(frame, uv + vec2(-f, -f), f).xyz * 0.125
        +  gauss(frame, uv + vec2(0, -f), f).xyz * 0.125
        +  gauss(frame, uv + vec2(f, -f), f).xyz * 0.125;
}

vec3 gauss3(sampler2D frame, vec2 uv, float f)
{
    return gauss2(frame, uv + vec2(f, 0), f).xyz * 0.125
        +  gauss2(frame, uv + vec2(f, f), f).xyz * 0.125
        +  gauss2(frame, uv + vec2(0, f), f).xyz * 0.125
        +  gauss2(frame, uv + vec2(-f, f), f).xyz * 0.125
        +  gauss2(frame, uv + vec2(-f, 0), f).xyz * 0.125
        +  gauss2(frame, uv + vec2(-f, -f), f).xyz * 0.125
        +  gauss2(frame, uv + vec2(0, -f), f).xyz * 0.125
        +  gauss2(frame, uv + vec2(f, -f), f).xyz * 0.125;
}

vec3 gauss4(sampler2D frame, vec2 uv, float f)
{
    return gauss3(frame, uv + vec2(f, 0), f).xyz * 0.125
        +  gauss3(frame, uv + vec2(f, f), f).xyz * 0.125
        +  gauss3(frame, uv + vec2(0, f), f).xyz * 0.125
        +  gauss3(frame, uv + vec2(-f, f), f).xyz * 0.125
        +  gauss3(frame, uv + vec2(-f, 0), f).xyz * 0.125
        +  gauss3(frame, uv + vec2(-f, -f), f).xyz * 0.125
        +  gauss3(frame, uv + vec2(0, -f), f).xyz * 0.125
        +  gauss3(frame, uv + vec2(f, -f), f).xyz * 0.125;
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

int read(sampler2D frame, vec2 uv, float k, int d, float t)
{
    float inv_k = 1 / k;
    vec2 frame_uv = floor(uv * k) * inv_k;
    frame_uv += vec2(d % 2, floor(d * 0.5)) * 0.5 * inv_k;
    return // TODO threshold
        ((mean(reframe(frame, frame_uv + vec2(0, 3) * inv_k * 0.125)) > t) ? 1 : 0) + 
        ((mean(reframe(frame, frame_uv + vec2(0, 2) * inv_k * 0.125)) > t) ? 2 : 0) +
        ((mean(reframe(frame, frame_uv + vec2(0, 1) * inv_k * 0.125)) > t) ? 4 : 0) +
        ((mean(reframe(frame, frame_uv + vec2(0, 0) * inv_k * 0.125)) > t) ? 8 : 0) +

        ((mean(reframe(frame, frame_uv + vec2(1, 3) * inv_k * 0.125)) > t) ? 16 : 0) +
        ((mean(reframe(frame, frame_uv + vec2(1, 2) * inv_k * 0.125)) > t) ? 32 : 0) +
        ((mean(reframe(frame, frame_uv + vec2(1, 1) * inv_k * 0.125)) > t) ? 64 : 0) +
        ((mean(reframe(frame, frame_uv + vec2(1, 0) * inv_k * 0.125)) > t) ? 128 : 0) +

        ((mean(reframe(frame, frame_uv + vec2(2, 3) * inv_k * 0.125)) > t) ? 256 : 0) +
        ((mean(reframe(frame, frame_uv + vec2(2, 2) * inv_k * 0.125)) > t) ? 512 : 0) +
        ((mean(reframe(frame, frame_uv + vec2(2, 1) * inv_k * 0.125)) > t) ? 1024 : 0) +
        ((mean(reframe(frame, frame_uv + vec2(2, 0) * inv_k * 0.125)) > t) ? 2048 : 0) +

        ((mean(reframe(frame, frame_uv + vec2(3, 3) * inv_k * 0.125)) > t) ? 4096 : 0) +
        ((mean(reframe(frame, frame_uv + vec2(3, 2) * inv_k * 0.125)) > t) ? 8192 : 0) +
        ((mean(reframe(frame, frame_uv + vec2(3, 1) * inv_k * 0.125)) > t) ? 16384 : 0) +
        ((mean(reframe(frame, frame_uv + vec2(3, 0) * inv_k * 0.125)) > t) ? 32768 : 0);
}

// https://web.archive.org/web/20151229003112/http://blogs.msdn.com/b/jeuge/archive/2005/06/08/hakmem-bit-count.aspx
int bit_count(int u)                         
{
        int c;
        c = u - ((u >> 1) & 033333333333) - ((u >> 2) & 011111111111);
        return ((c + (c >> 3)) & 030707070707) % 63;
}

int guess_char(sampler2D frame, vec2 uv, float k, float t)
{
    int b0 = read(frame, uv, k, 0, t);
    int b1 = read(frame, uv, k, 1, t);
    int b2 = read(frame, uv, k, 2, t);
    int b3 = read(frame, uv, k, 3, t);
    
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

// TODO

// 6. effects
// ----------

// TODO