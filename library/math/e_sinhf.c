/*
 * $Id: math_e_sinhf.c,v 1.3 2023-07-19 12:04:24 clib4devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float one = 1.0, shuge = 1.0e37;

float
__ieee754_sinhf(float x) {
    float t, h;
    int32_t ix, jx;

    GET_FLOAT_WORD(jx, x);
    ix = jx & 0x7fffffff;

    /* x is INF or NaN */
    if (ix >= 0x7f800000) return x + x;

    h = 0.5;
    if (jx < 0) h = -h;
    /* |x| in [0,9], return sign(x)*0.5*(E+E/(E+1))) */
    if (ix < 0x41100000) {        /* |x|<9 */
        if (ix < 0x39800000)        /* |x|<2**-12 */
            if (shuge + x > one) return x;/* sinh(tiny) = tiny with inexact */
        t = expm1f(fabsf(x));
        if (ix < 0x3f800000) return h * ((float) 2.0 * t - t * t / (t + one));
        return h * (t + t / (t + one));
    }

    /* |x| in [9, logf(maxfloat)] return 0.5*exp(|x|) */
    if (ix < 0x42b17217) return h * __ieee754_expf(fabsf(x));

    /* |x| in [logf(maxfloat), overflowthresold] */
    if (ix <= 0x42b2d4fc)
        return h * 2.0F * __ldexp_expf(fabsf(x), -1);

    /* |x| > overflowthresold, sinh(x) overflow */
    return x * shuge;
}
