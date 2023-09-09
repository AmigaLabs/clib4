/*
 * $Id: math_s_frexpf.c,v 1.4 2023-07-19 12:04:23 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float two25 = 3.3554432000e+07; /* 0x4c000000 */

float
frexpf(float x, int *eptr) {
    int32_t hx, ix;
    GET_FLOAT_WORD(hx, x);
    ix = 0x7fffffff & hx;
    *eptr = 0;
    if (ix >= 0x7f800000 || (ix == 0)) return x;    /* 0,inf,nan */
    if (ix < 0x00800000) {        /* subnormal */
        x *= two25;
        GET_FLOAT_WORD(hx, x);
        ix = hx & 0x7fffffff;
        *eptr = -25;
    }
    *eptr += (ix >> 23) - 126;
    hx = (hx & 0x807fffff) | 0x3f000000;
    SET_FLOAT_WORD(x, hx);
    return x;
}
