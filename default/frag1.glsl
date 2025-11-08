#version 460

// VIDEO 1
// -----------
// IN: 1 (RAW IN A)
// OUT: 3 (IN A)

in vec2 vUV;
out vec4 fragColor;

#include inc_yuv.glsl

uniform sampler2D iTex0;
uniform sampler2D iTex1;
uniform int iInputFormat1;
uniform vec2 iInputResolution1;

void main() {
    if (iInputFormat1 == YUYV_FOURCC) {
        fragColor = yuyvTex(iTex1, vUV, int(iInputResolution1.x));
    } else {
        fragColor = texture(iTex0, vUV);
    }
}