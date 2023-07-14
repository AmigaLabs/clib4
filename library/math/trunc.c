/*
 * $Id: math_trunc.c,v 1.4 2023-07-14 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
trunc(double x) {
    int32_t i0, j0;
    uint32_t i1;
    int sx;

    EXTRACT_WORDS (i0, i1, x);
    sx = i0 & 0x80000000;
    j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
    if (j0 < 20) {
        if (j0 < 0)
            /* The magnitude of the number is < 1 so the result is +-0.  */
            INSERT_WORDS (x, sx, 0);
        else
            INSERT_WORDS (x, sx | (i0 & ~(0x000fffff >> j0)), 0);
    } else if (j0 > 51) {
        if (j0 == 0x400)
            /* x is inf or NaN.  */
            return x + x;
    } else {
        INSERT_WORDS (x, i0, i1 & ~(0xffffffffu >> (j0 - 20)));
    }

    return x;
}
