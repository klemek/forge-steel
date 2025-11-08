#ifndef PI
#define PI 3.1415927
#endif

#include inc_rand.glsl

#ifndef INC_TIME
#define INC_TIME

uniform float iTime;
uniform float iTempo;
uniform float iBeats;

float randTime(float seed){
    return rand(seed + floor(iBeats / 4));
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
    return mod(divider(k) * iBeats * k2 / 4, 1);
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

#endif