#ifndef _Math_H_
#define _Math_H_

#include <math.h>

#define INF 1212123.0

namespace Math {
    const float PI = 3.1415926;

    inline float radians(float r) {
        return r * (PI / 180);
    }

    inline float cot(float angle) {
        return 1 / tan(radians(angle));
    }

    inline float clamp(float a, float b, float x) {
        if (x <= a) return a;
        if (x >= b) return b;
        return x;
    }
}


#endif 