#include frag0.glsl

// VIDEO 1
// -----------
// IN: 1 (RAW IN A)
// OUT: 3 (IN A)

in vec2 vUV;
out vec4 fragColor;

void main() {
    if (iInputFormat1 == YUYV_FOURCC) {
        fragColor = yuyvTex(iTex1, vUV, int(iInputResolution1.x));
    } else {
        fragColor = texture(iTex0, vUV);
    }
}