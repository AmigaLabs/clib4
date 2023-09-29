/*
 * $Id: math_s_tan.c,v 1.8 2023-07-19 12:04:24 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
tan(double x) {
    double y[2], z = 0.0;
    int32_t n, ix;

    /* High word of x. */
    GET_HIGH_WORD(ix, x);

    /* |x| ~< pi/4 */
    ix &= 0x7fffffff;
    if (ix <= 0x3fe921fb) {
        if (ix < 0x3e400000)            /* x < 2**-27 */
            if ((int) x == 0) return x;        /* generate inexact */
        return __kernel_tan(x, z, 1);
    }

        /* tan(Inf or NaN) is NaN */
    else if (ix >= 0x7ff00000) return x - x;        /* NaN */

        /* argument reduction needed */
    else {
        n = __ieee754_rem_pio2(x, y);
        return __kernel_tan(y[0], y[1], 1 - ((n & 1) << 1)); /*   1 -- n even
							-1 -- n odd */
    }
}
