/*
 * $Id: math_frexp.c,v 1.6 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double two54 = _F_64(1.80143985094819840000e+16); /* 0x43500000, 0x00000000 */

double
frexp(double x, int *eptr) {
    int32_t hx, ix, lx;
    EXTRACT_WORDS(hx, lx, x);
    ix = 0x7fffffff & hx;
    *eptr = 0;
    if (ix >= 0x7ff00000 || ((ix | lx) == 0))
        return x + x; /* 0,inf,nan */
    if (ix < 0x00100000) { /* subnormal */
        x *= two54;
        GET_HIGH_WORD(hx, x);
        ix = hx & 0x7fffffff;
        *eptr = -54;
    }
    *eptr += (ix >> 20) - 1022;
    hx = (hx & 0x800fffff) | 0x3fe00000;
    SET_HIGH_WORD(x, hx);
    return x;
}
