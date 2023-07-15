/*
 * $Id: math_tanf.c,v 1.4 2022-07-15 12:04:24 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float
tanf(float x) {
    float y[2], z = 0.0;
    int32_t n, ix;

    GET_FLOAT_WORD(ix, x);

    /* |x| ~< pi/4 */
    ix &= 0x7fffffff;
    if (ix <= 0x3f490fda) {
        return __kernel_tanf(x, z, 1);
        /* tan(Inf or NaN) is NaN */
    }
    else if (ix >= 0x7f800000) {
        if (ix == 0x7f800000)
            __set_errno(EDOM);
        return x - x;        /* NaN */
    }
    /* argument reduction needed */
    else {
        n = __rem_pio2f(x, y);
        return __kernel_tanf(y[0], y[1], 1 - ((n & 1) << 1)); /*   1 -- n even -1 -- n odd */
    }
}
