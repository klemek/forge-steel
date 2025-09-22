// VIDEO 1
// -----------
// IN: 1 (RAW IN A)
// OUT: 3 (IN A)

in vec2 vUV;
out vec4 fragColor;

uniform vec3 iInputResolution1;

void main() {
    if (iInputResolution1.z == YUYV_FOURCC) {
        fragColor = yuyvTex(tex1, vUV, int(iInputResolution1.x));
    } else {
        fragColor = vec4(0, 0, 0, 1);
    }
}