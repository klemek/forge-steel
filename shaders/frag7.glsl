// A+B
// ------------
// IN: 7 (FX A)
// IN: 8 (FX B)
// OUT: 9 (MFX)

in vec2 vUV;
out vec4 fragColor;

void main() {
    float mix_src = magic(seed7);
    bool key = magic_trigger(seed7 + 10);

    vec4 color_a = texture(tex7, vUV);
    vec4 color_b = texture(tex8, vUV);

    float k = mean(color_a);

    fragColor = mix(color_b, color_a, key ? step(mix_src, k) : mix_src);
}