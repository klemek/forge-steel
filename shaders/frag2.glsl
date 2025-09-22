// VIDEO 2
// -----------
// IN: 2 (RAW IN B)
// OUT: 4 (IN B)

in vec2 vUV;
out vec4 fragColor;

uniform vec3 iInputResolution2;

void main() {
    if (iInputResolution2.z == YUYV_FOURCC) {
        fragColor = yuyvTex(tex2, vUV, int(iInputResolution2.x));
    } else {
        fragColor = vec4(0, 0, 0, 1);
    }
}