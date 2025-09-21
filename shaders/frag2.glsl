// VIDEO 2
// -----------
// IN: 2 (RAW IN B)
// OUT: 4 (IN B)

in vec2 vUV;
out vec4 fragColor;

void main() {
    float r, g, b, y, u, v;
    vec4 src = texture(tex2, vec2(vUV.x, 1 - vUV.y));
    y = src.r;
    u = src.g - 0.5;
    v = src.a - 0.5;
    r = y + 1.13983 * v;
    g = y - 0.39465 * u - 0.58060 * v;
    b = y + 2.03211 * u;
    fragColor = vec4(r, g, b, 1.0);
}