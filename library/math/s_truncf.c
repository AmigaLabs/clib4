/*
 * $Id: math_s_truncf.c,v 1.4 2023-07-18 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float huge = 1.0e30F;

float
truncf(float x) {
    int32_t i0, j0;
    uint32_t i;
    GET_FLOAT_WORD(i0, x);
    j0 = ((i0 >> 23) & 0xff) - 0x7f;
    if (j0 < 23) {
        if (j0 < 0) {    /* raise inexact if x != 0 */
            if (huge + x > 0.0F)        /* |x|<1, so return 0*sign(x) */
                i0 &= 0x80000000;
        } else {
            i = (0x007fffff) >> j0;
            if ((i0 & i) == 0) return x; /* x is integral */
            if (huge + x > 0.0F)        /* raise inexact flag */
                i0 &= (~i);
        }
    } else {
        if (j0 == 0x80) return x + x;    /* inf or NaN */
        else return x;        /* x is integral */
    }
    SET_FLOAT_WORD(x, i0);
    return x;
}
