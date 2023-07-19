/*
 * $Id: math_s_ctanhf.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float complex
ctanhf(float complex z) {
    float x, y;
    float t, beta, s, rho, denom;
    uint32_t hx, ix;

    x = crealf(z);
    y = cimagf(z);

    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;

    if (ix >= 0x7f800000) {
        if (ix & 0x7fffff)
            return (CMPLXF(x, (y == 0 ? y : x * y)));
        SET_FLOAT_WORD(x, hx - 0x40000000);
        return (CMPLXF(x,
                       copysignf(0, isinf(y) ? y : sinf(y) * cosf(y))));
    }

    if (!isfinite(y))
        return (CMPLXF(y - y, y - y));

    if (ix >= 0x41300000) {    /* x >= 11 */
        float exp_mx = expf(-fabsf(x));
        return (CMPLXF(copysignf(1, x),
                       4 * sinf(y) * cosf(y) * exp_mx * exp_mx));
    }

    t = tanf(y);
    beta = 1.0 + t * t;
    s = sinhf(x);
    rho = sqrtf(1 + s * s);
    denom = 1 + beta * s * s;
    return (CMPLXF((beta * rho * s) / denom, t / denom));
}
