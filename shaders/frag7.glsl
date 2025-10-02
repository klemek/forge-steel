// A+B
// ------------
// IN: 7 (FX A)
// IN: 8 (FX B)
// OUT: 9 (MFX)

in vec2 vUV;
out vec4 fragColor;

void main() {
    float mix_value = magic(src3_1[1].xy, vec3(1, 0, 0), seed7);
    bool mix_type = magic_trigger(vec3(src3_1[0].x, 0, 0), seed7 + 10);

    vec4 color_a = texture(tex7, vUV);
    vec4 color_b = texture(tex8, vUV);

    float k = mean(color_a);

    fragColor = mix(color_b, color_a, mix_type ? step(mix_value, k) : mix_value);
}