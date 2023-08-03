/*
 * $Id: math_s_modff.c,v 1.2 2023-07-19 12:04:23 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float one = 1.0;

float
modff(float x, float *iptr) {
    int32_t i0, j0;
    uint32_t i;
    GET_FLOAT_WORD(i0, x);
    j0 = ((i0 >> 23) & 0xff) - 0x7f;    /* exponent of x */
    if (j0 < 23) {            /* integer part in x */
        if (j0 < 0) {            /* |x|<1 */
            SET_FLOAT_WORD(*iptr, i0 & 0x80000000);    /* *iptr = +-0 */
            return x;
        } else {
            i = (0x007fffff) >> j0;
            if ((i0 & i) == 0) {            /* x is integral */
                uint32_t ix;
                *iptr = x;
                GET_FLOAT_WORD(ix, x);
                SET_FLOAT_WORD(x, ix & 0x80000000);    /* return +-0 */
                return x;
            } else {
                SET_FLOAT_WORD(*iptr, i0 & (~i));
                return x - *iptr;
            }
        }
    } else {            /* no fraction part */
        uint32_t ix;
        *iptr = x * one;
        if (x != x)            /* NaN */
            return x;
        GET_FLOAT_WORD(ix, x);
        SET_FLOAT_WORD(x, ix & 0x80000000);    /* return +-0 */
        return x;
    }
}
