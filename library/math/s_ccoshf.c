/*
 * $Id: math_s_cccoshf.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float huge = 0x1p127;

float complex
ccoshf(float complex z) {
    float x, y, h;
    int32_t hx, hy, ix, iy;

    x = crealf(z);
    y = cimagf(z);

    GET_FLOAT_WORD(hx, x);
    GET_FLOAT_WORD(hy, y);

    ix = 0x7fffffff & hx;
    iy = 0x7fffffff & hy;

    if (ix < 0x7f800000 && iy < 0x7f800000) {
        if (iy == 0)
            return (CMPLXF(coshf(x), x * y));
        if (ix < 0x41100000)    /* small x: normal case */
            return (CMPLXF(coshf(x) * cosf(y), sinhf(x) * sinf(y)));

        /* |x| >= 9, so cosh(x) ~= exp(|x|) */
        if (ix < 0x42b17218) {
            /* x < 88.7: expf(|x|) won't overflow */
            h = expf(fabsf(x)) * 0.5f;
            return (CMPLXF(h * cosf(y), copysignf(h, x) * sinf(y)));
        } else if (ix < 0x4340b1e7) {
            /* x < 192.7: scale to avoid overflow */
            z = __ldexp_cexpf(CMPLXF(fabsf(x), y), -1);
            return (CMPLXF(crealf(z), cimagf(z) * copysignf(1, x)));
        } else {
            /* x >= 192.7: the result always overflows */
            h = huge * x;
            return (CMPLXF(h * h * cosf(y), h * sinf(y)));
        }
    }

    if (ix == 0 && iy >= 0x7f800000)
        return (CMPLXF(y - y, copysignf(0, x * (y - y))));

    if (iy == 0 && ix >= 0x7f800000) {
        if ((hx & 0x7fffff) == 0)
            return (CMPLXF(x * x, copysignf(0, x) * y));
        return (CMPLXF(x * x, copysignf(0, (x + x) * y)));
    }

    if (ix < 0x7f800000 && iy >= 0x7f800000)
        return (CMPLXF(y - y, x * (y - y)));

    if (ix >= 0x7f800000 && (hx & 0x7fffff) == 0) {
        if (iy >= 0x7f800000)
            return (CMPLXF(x * x, x * (y - y)));
        return (CMPLXF((x * x) * cosf(y), x * sinf(y)));
    }

    return (CMPLXF((x * x) * (y - y), (x + x) * (y - y)));
}
