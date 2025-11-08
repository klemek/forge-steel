#ifndef INC_RAND
#define INC_RAND

float rand(float seed){
    float v=pow(abs(seed),6./7.);
    v*=sin(v)+1.;
    return fract(v);
}

float rand(vec2 n){
    return rand(n.x * 1234 + n.y * 9876);
}

#endif