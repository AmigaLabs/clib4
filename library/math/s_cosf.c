/*
 * $Id: math_s_cosf.c,v 1.4 2023-07-15 12:04:23 clib4devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

/* Small multiples of pi/2 rounded to double precision. */
static const double
        c1pio2 = 1 * M_PI_2,            /* 0x3FF921FB, 0x54442D18 */
        c2pio2 = 2 * M_PI_2,            /* 0x400921FB, 0x54442D18 */
        c3pio2 = 3 * M_PI_2,            /* 0x4012D97C, 0x7F3321D2 */
        c4pio2 = 4 * M_PI_2;            /* 0x401921FB, 0x54442D18 */

float
cosf(float x) {
    double y;
    int32_t n, hx, ix;

    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;

    if (ix <= 0x3f490fda) {        /* |x| ~<= pi/4 */
        if (ix < 0x39800000)        /* |x| < 2**-12 */
            if (((int) x) == 0) return 1.0;    /* 1 with inexact if x != 0 */
        return __kernel_cosdf(x);
    }
    if (ix <= 0x407b53d1) {        /* |x| ~<= 5*pi/4 */
        if (ix <= 0x4016cbe3) {    /* |x|  ~> 3*pi/4 */
            if (hx > 0)
                return __kernel_sindf(c1pio2 - x);
            else
                return __kernel_sindf(x + c1pio2);
        } else
            return -__kernel_cosdf(x + (hx > 0 ? -c2pio2 : c2pio2));
    }
    if (ix <= 0x40e231d5) {        /* |x| ~<= 9*pi/4 */
        if (ix <= 0x40afeddf) {    /* |x|  ~> 7*pi/4 */
            if (hx > 0)
                return __kernel_sindf(x - c3pio2);
            else
                return __kernel_sindf(-c3pio2 - x);
        } else
            return __kernel_cosdf(x + (hx > 0 ? -c4pio2 : c4pio2));
    }

        /* cos(Inf or NaN) is NaN */
    else if (ix >= 0x7f800000) return x - x;

        /* general argument reduction needed */
    else {
        n = __ieee754_rem_pio2f(x, &y);
        switch (n & 3) {
            case 0:
                return __kernel_cosdf(y);
            case 1:
                return __kernel_sindf(-y);
            case 2:
                return -__kernel_cosdf(y);
            default:
                return __kernel_sindf(y);
        }
    }
}
