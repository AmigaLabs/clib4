/*
 * $Id: math_round.c,v 1.4 2023-07-14 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const __float64 huge = _F_64(1.0e300);

double
round(double x) {
    int32_t i0, j0;
    uint32_t i1;

    EXTRACT_WORDS (i0, i1, x);
    j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
    if (j0 < 20) {
        if (j0 < 0) {
            math_force_eval (huge + x);

            i0 &= 0x80000000;
            if (j0 == -1)
                i0 |= 0x3ff00000;
            i1 = 0;
        } else {
            uint32_t i = 0x000fffff >> j0;
            if (((i0 & i) | i1) == 0)
                /* X is integral.  */
                return x;
            math_force_eval (huge + x);

            /* Raise inexact if x != 0.  */
            i0 += 0x00080000 >> j0;
            i0 &= ~i;
            i1 = 0;
        }
    } else if (j0 > 51) {
        if (j0 == 0x400)
            /* Inf or NaN.  */
            return x + x;
        else
            return x;
    } else {
        uint32_t i = 0xffffffff >> (j0 - 20);
        if ((i1 & i) == 0)
            /* X is integral.  */
            return x;

        math_force_eval (huge + x);

        /* Raise inexact if x != 0.  */
        uint32_t j = i1 + (1 << (51 - j0));
        if (j < i1)
            i0 += 1;
        i1 = j;
        i1 &= ~i;
    }

    INSERT_WORDS (x, i0, i1);
    return x;
}
