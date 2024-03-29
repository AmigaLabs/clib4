/*
 * $Id: math_s_tanf.c,v 1.5 2023-08-18 12:04:24 clib4devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

/* Small multiples of pi/2 rounded to double precision. */
static const double
        t1pio2 = 1 * M_PI_2,            /* 0x3FF921FB, 0x54442D18 */
        t2pio2 = 2 * M_PI_2,            /* 0x400921FB, 0x54442D18 */
        t3pio2 = 3 * M_PI_2,            /* 0x4012D97C, 0x7F3321D2 */
        t4pio2 = 4 * M_PI_2;            /* 0x401921FB, 0x54442D18 */

float
tanf(float x) {
    double y;
    int32_t n, hx, ix;

    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;

    if (ix <= 0x3f490fda) {        /* |x| ~<= pi/4 */
        if (ix < 0x39800000)        /* |x| < 2**-12 */
            if (((int) x) == 0) return x;    /* x with inexact if x != 0 */
        return __kernel_tandf(x, 1);
    }
    if (ix <= 0x407b53d1) {        /* |x| ~<= 5*pi/4 */
        if (ix <= 0x4016cbe3)        /* |x| ~<= 3pi/4 */
            return __kernel_tandf(x + (hx > 0 ? -t1pio2 : t1pio2), -1);
        else
            return __kernel_tandf(x + (hx > 0 ? -t2pio2 : t2pio2), 1);
    }
    if (ix <= 0x40e231d5) {        /* |x| ~<= 9*pi/4 */
        if (ix <= 0x40afeddf)        /* |x| ~<= 7*pi/4 */
            return __kernel_tandf(x + (hx > 0 ? -t3pio2 : t3pio2), -1);
        else
            return __kernel_tandf(x + (hx > 0 ? -t4pio2 : t4pio2), 1);
    }

        /* tan(Inf or NaN) is NaN */
    else if (ix >= 0x7f800000) return x - x;

        /* general argument reduction needed */
    else {
        n = __ieee754_rem_pio2f(x, &y);
        /* integer parameter: 1 -- n even; -1 -- n odd */
        return __kernel_tandf(y, 1 - ((n & 1) << 1));
    }
}