/*
 * $Id: math_ceilf.c,v 1.5 2023-07-14 12:04:23 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float huge = 1.0e30;

float
ceilf(float x) {
    int32_t i0, j0;
    uint32_t i;

    GET_FLOAT_WORD(i0, x);
    j0 = ((i0 >> 23) & 0xff) - 0x7f;
    if (j0 < 23) {
        if (j0 < 0) {    /* raise inexact if x != 0 */
            math_force_eval(huge + x);/* return 0*sign(x) if |x|<1 */
            if (i0 < 0) { i0 = 0x80000000; }
            else if (i0 != 0) { i0 = 0x3f800000; }
        } else {
            i = (0x007fffff) >> j0;
            if ((i0 & i) == 0) return x; /* x is integral */
            math_force_eval(huge + x);    /* raise inexact flag */
            if (i0 > 0) i0 += (0x00800000) >> j0;
            i0 &= (~i);
        }
    } else {
        if (__builtin_expect(j0 == 0x80, 0)) return x + x; /* inf or NaN */
        else return x;        /* x is integral */
    }
    SET_FLOAT_WORD(x, i0);
    return x;
}
