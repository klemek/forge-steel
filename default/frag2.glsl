#include frag0.glsl

// VIDEO 2
// -----------
// IN: 2 (RAW IN B)
// OUT: 4 (IN B)

in vec2 vUV;
out vec4 fragColor;

void main() {
    if (iInputFormat2 == YUYV_FOURCC) {
        fragColor = yuyvTex(iTex2, vUV, int(iInputResolution2.x));
    } else {
        fragColor = texture(iTex0, vUV);
    }
}