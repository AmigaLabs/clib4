/*
 * $Id: math_rintf.c,v 1.4 2022-03-12 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float
        TWO23[2] = {
        8.3886080000e+06, /* 0x4b000000 */
        -8.3886080000e+06, /* 0xcb000000 */
        };

float rintf(float x) {
    int32_t i0, j0, sx;
    float w, t;
    GET_FLOAT_WORD(i0, x);
    sx = (i0 >> 31) & 1;
    j0 = ((i0 >> 23) & 0xff) - 0x7f;
    if (j0 < 23) {
        if (j0 < 0) {
            if ((i0 & 0x7fffffff) == 0) return x;
            STRICT_ASSIGN(float, w, TWO23[sx] + x);
            t = w - TWO23[sx];
            GET_FLOAT_WORD(i0, t);
            SET_FLOAT_WORD(t, (i0 & 0x7fffffff) | (sx << 31));
            return t;
        }
        STRICT_ASSIGN(float, w, TWO23[sx] + x);
        return w - TWO23[sx];
    }
    if (j0 == 0x80) return x + x;    /* inf or NaN */
    else return x;            /* x is integral */
}
