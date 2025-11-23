#version 460

// VIDEO 2
// -----------
// IN: 2 (RAW IN B)
// OUT: 4 (IN B)

in vec2 vUV;
out vec4 fragColor;

#include inc_yuyv.glsl

uniform sampler2D iTex0;
uniform sampler2D iTex2;
uniform int iInputFormat2;
uniform vec2 iInputResolution2;

void main() {
    if (iInputFormat2 == YUYV_FOURCC) {
        fragColor = yuyvTex(iTex2, vUV, int(iInputResolution2.x));
    } else if (iInputResolution2.x > 0) {
        fragColor = texture(iTex2, vUV);
    } else {
        fragColor = texture(iTex0, vUV);
    }
}