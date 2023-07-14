/*
 * $Id: math_roundf.c,v 1.4 2023-07-14 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float huge = 1.0e30;

float
roundf(float x) {
    int32_t i0, j0;

    GET_FLOAT_WORD (i0, x);
    j0 = ((i0 >> 23) & 0xff) - 0x7f;
    if (j0 < 23) {
        if (j0 < 0) {
            math_force_eval (huge + x);

            i0 &= 0x80000000;
            if (j0 == -1)
                i0 |= 0x3f800000;
        } else {
            uint32_t i = 0x007fffff >> j0;
            if ((i0 & i) == 0)
                /* X is integral.  */
                return x;
            math_force_eval (huge + x);

            /* Raise inexact if x != 0.  */
            i0 += 0x00400000 >> j0;
            i0 &= ~i;
        }
    } else {
        if (j0 == 0x80)
            /* Inf or NaN.  */
            return x + x;
        else
            return x;
    }

    SET_FLOAT_WORD (x, i0);
    return x;
}
