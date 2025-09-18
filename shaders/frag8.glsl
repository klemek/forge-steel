// MONITOR
// ---

in vec2 vUV;
out vec4 fragColor;

float s(vec2 uv, float x0, float y0) {
    return step(x0, uv.x) * step(-x0 - 1, -uv.x) * step(y0, uv.y) *
step(-y0 - 1, -uv.y);
}

void main() {
    vec2 uv = vUV * 3;
    fragColor = vec4(0);
    fragColor += s(uv,0,2) * texture(tex1, uv);
    fragColor += s(uv,1,2) * texture(tex2, uv);
    fragColor += s(uv,2,2) * texture(tex3, uv);
    fragColor += s(uv,0,1) * texture(tex4, uv);
    fragColor += s(uv,1,1) * texture(tex5, uv);
    fragColor += s(uv,2,1) * texture(tex6, uv);
    fragColor += s(uv,0,0) * texture(tex7, uv);
    fragColor += s(uv,1,0) * texture(tex8, uv);
    fragColor += s(uv,2,0) * texture(tex0, uv);
}