/*
 * $Id: math_e_acosh.c,v 1.4 2023-07-19 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double
        one = 1.0,
        ln2 = 6.93147180559945286227e-01;  /* 0x3FE62E42, 0xFEFA39EF */

double
__ieee754_acosh(double x) {
    double t;
    int32_t hx;
    uint32_t lx;
    EXTRACT_WORDS(hx, lx, x);
    if (hx < 0x3ff00000) {        /* x < 1 */
        return (x - x) / (x - x);
    } else if (hx >= 0x41b00000) {    /* x > 2**28 */
        if (hx >= 0x7ff00000) {    /* x is inf of NaN */
            return x + x;
        } else
            return __ieee754_log(x) + ln2;    /* acosh(huge)=log(2x) */
    } else if (((hx - 0x3ff00000) | lx) == 0) {
        return 0.0;            /* acosh(1) = 0 */
    } else if (hx > 0x40000000) {    /* 2**28 > x > 2 */
        t = x * x;
        return __ieee754_log(2.0 * x - one / (x + sqrt(t - one)));
    } else {            /* 1<x<2 */
        t = x - one;
        return log1p(t + sqrt(2.0 * t + t * t));
    }
}
