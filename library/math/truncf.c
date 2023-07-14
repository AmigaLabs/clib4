/*
 * $Id: math_truncf.c,v 1.4 2023-07-14 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float
truncf(float x) {
    int32_t i0, j0;
    int sx;

    GET_FLOAT_WORD (i0, x);
    sx = i0 & 0x80000000;
    j0 = ((i0 >> 23) & 0xff) - 0x7f;
    if (j0 < 23) {
        if (j0 < 0)
            /* The magnitude of the number is < 1 so the result is +-0.  */
            SET_FLOAT_WORD (x, sx);
        else
            SET_FLOAT_WORD (x, sx | (i0 & ~(0x007fffff >> j0)));
    } else {
        if (j0 == 0x80)
            /* x is inf or NaN.  */
            return x + x;
    }

    return x;
}
