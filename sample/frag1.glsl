#version 460

#define PI 3.1415927

in vec2 vUV;
out vec4 fragColor;

uniform float iTime; // elapsed time in seconds
uniform float iTempo; // current tempo in bpm
uniform float iBeats; // elapsed beats since last tempo reset
uniform int iFPS; // output window frames per seconds
uniform int iDemo; // 0/1 if demo mode
uniform int iSeed1; // a random seed assigned at start
uniform vec2 iResolution; // output window resolution in pixels
uniform vec3 iMidi1_1[20]; // all midi inputs defined
uniform sampler2D iTex0; // available texture (this code output, so feedback)

void main() {
    // all available buttons and faders
    vec3 b1 = iMidi1_1[0];
    vec2 f1 = iMidi1_1[1].xy;
    vec3 b2 = iMidi1_1[2];
    vec2 f2 = iMidi1_1[3].xy;
    vec3 b3 = iMidi1_1[4];
    vec2 f3 = iMidi1_1[5].xy;
    vec3 b4 = iMidi1_1[6];
    vec2 f4 = iMidi1_1[7].xy;
    vec3 b5 = iMidi1_1[8];
    vec2 f5 = iMidi1_1[9].xy;
    vec3 b6 = iMidi1_1[10];
    vec2 f6 = iMidi1_1[11].xy;
    vec3 b7 = iMidi1_1[12];
    vec2 f7 = iMidi1_1[13].xy;
    vec3 b8 = iMidi1_1[14];
    vec2 f8 = iMidi1_1[15].xy;
    vec3 b9 = iMidi1_1[16];
    vec3 b10 = iMidi1_1[17];
    vec3 b11 = iMidi1_1[18];
    vec3 b12 = iMidi1_1[19];

    // center UV and scale it to ratio
    vec2 uv0 = vUV.st;
    float ratio = iResolution.x / iResolution.y;
    vec2 uv1 = (uv0 - .5) * vec2(ratio, 1);

    // make small circle controlled by fader 1 and 2
    float circle_dist = 0.1 + 0.9 * f1.x;
    float circle_size = 0.1 + 0.9 * f2.x;
    vec2 circle_pos = circle_dist * vec2(
        sin(iBeats * 2 * PI),
        cos(iBeats * 2 * PI)
    );
    float circle = 1 - step(circle_size, length(uv1 - circle_pos));

    // fader 3, 4 and 5 controls the color
    vec3 color = vec3(
        f3.x,
        f4.x,
        f5.x
    );
    vec3 out_color = mix(color, 1 - color, circle);

    // fader 8 controls the feedback value
    fragColor = mix(vec4(out_color, 1), texture(iTex0, vUV.st), f8.x);
}