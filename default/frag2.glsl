#version 460

// VIDEO 2
// -----------
// IN: 2+4 (RAW IN B)
// OUT: 6 (IN B)

in vec2 vUV;
out vec4 fragColor;

#include inc_yuyv.glsl

uniform sampler2D iTex0;
uniform sampler2D iTex2;
uniform sampler2D iTex4;
uniform int iInputFormat2;
uniform int iInputSwap2;
uniform vec2 iInputResolution2;

void main() {
    if (iInputFormat2 == YUYV_FOURCC) {
        if (iInputSwap2 > 0) {
            fragColor = yuyvTex(iTex4, vUV, int(iInputResolution2.x));
        } else {
            fragColor = yuyvTex(iTex2, vUV, int(iInputResolution2.x));
        }
    } else {
        fragColor = texture(iTex0, vUV);
    }
}