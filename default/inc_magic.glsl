#include inc_time.glsl

#ifndef INC_MAGIC
#define INC_MAGIC

uniform int iDemo;

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

float fmagic(vec2 F, vec3 B, float i)
{
    vec2 f = magic_f(F, B, i);
    vec3 b = magic_b(B, i);

    return f.x * mix(1 - modTime(f.y), cosTime(f.y) * 0.5 + 0.5, b.y);
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

#endif