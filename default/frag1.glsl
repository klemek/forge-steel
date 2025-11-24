#version 460

// VIDEO 1
// -----------
// IN: 1+3 (RAW IN A)
// OUT: 5 (IN A)

in vec2 vUV;
out vec4 fragColor;

#include inc_yuyv.glsl

uniform sampler2D iTex0;
uniform sampler2D iTex1;
uniform sampler2D iTex3;
uniform int iInputFormat1;
uniform int iInputSwap1;
uniform vec2 iInputResolution1;

void main() {
    if (iInputFormat1 == YUYV_FOURCC) {
        if (iInputSwap1 > 0) {
            fragColor = yuyvTex(iTex3, vUV, int(iInputResolution1.x));
        } else {
            fragColor = yuyvTex(iTex1, vUV, int(iInputResolution1.x));
        }
    } else {
        fragColor = texture(iTex0, vUV);
    }
}