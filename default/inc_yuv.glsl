#ifndef INC_YUV
#define INC_YUV

const int YUYV_FOURCC = 1448695129;

const mat3x3 yuv_to_rgb = {{1,1,1},{0,-0.39465,2.03211},{1.13983,-0.5806,0}};

vec4 yuyvTex(sampler2D tex, vec2 vUV, int base_width) {
    float w = base_width - 1;
    
    int x = int(vUV.x * w);

    int xU = x - x % 2;
    int xV = x - x % 2 + 1;

    vec4 tU = texture(tex, vec2(xU / w, 1 - vUV.y));
    vec4 tV = texture(tex, vec2(xV / w, 1 - vUV.y));

    vec3 yuv = vec3(
        x % 2 == 0 ? tU.x : tV.x,
        tU.y - 0.5,
        tV.y - 0.5
    );

    return vec4(yuv_to_rgb * yuv, 1.0);
}

#endif