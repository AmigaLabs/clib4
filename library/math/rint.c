/*
 * $Id: math_rint.c,v 1.5 2023-07-14 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double TWO52[2] = {
    4.50359962737049600000e+15,  /* 0x43300000, 0x00000000 */
    -4.50359962737049600000e+15, /* 0xC3300000, 0x00000000 */
};

double
rint(double x) {
    int32_t i0, j0, sx;
    uint32_t i, i1;
    double w, t;
    EXTRACT_WORDS(i0, i1, x);
    sx = (i0 >> 31) & 1;
    j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
    if (j0 < 20) {
        if (j0 < 0) {
            if (((i0 & 0x7fffffff) | i1) == 0) return x;
            i1 |= (i0 & 0x0fffff);
            i0 &= 0xfffe0000;
            i0 |= ((i1 | -i1) >> 12) & 0x80000;
            SET_HIGH_WORD(x, i0);
            w = TWO52[sx] + x;
            t = w - TWO52[sx];
            GET_HIGH_WORD(i0, t);
            SET_HIGH_WORD(t, (i0 & 0x7fffffff) | (sx << 31));
            return t;
        } else {
            i = (0x000fffff) >> j0;
            if (((i0 & i) | i1) == 0) return x; /* x is integral */
            i >>= 1;
            if (((i0 & i) | i1) != 0) {
                if (j0 == 19)
                    i1 = 0x40000000;
                else if (j0 < 18)
                    i0 = (i0 & (~i)) | ((0x20000) >> j0);
                else {
                    i0 &= ~i;
                    i1 = 0x80000000;
                }
            }
        }
    } else if (j0 > 51) {
        if (j0 == 0x400) return x + x;    /* inf or NaN */
        else return x;        /* x is integral */
    } else {
        i = ((uint32_t)(0xffffffff)) >> (j0 - 20);
        if ((i1 & i) == 0) return x;    /* x is integral */
        i >>= 1;
        if ((i1 & i) != 0) i1 = (i1 & (~i)) | ((0x40000000) >> (j0 - 20));
    }
    INSERT_WORDS(x, i0, i1);
    w = TWO52[sx] + x;
    return w - TWO52[sx];
}
