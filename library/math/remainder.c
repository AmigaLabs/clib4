/*
 * $Id: math_remainder.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double zero = 0.0;

double
remainder(double x, double p) {
    LONG hx, hp;
    ULONG sx, lx, lp;
    double p_half;

    EXTRACT_WORDS(hx, lx, x);
    EXTRACT_WORDS(hp, lp, p);
    sx = hx & 0x80000000U;
    hp &= 0x7fffffff;
    hx &= 0x7fffffff;

    /* purge off exception values */
    if ((hp | lp) == 0)
        return (x * p) / (x * p); /* p = 0 */
    if ((hx >= 0x7ff00000) ||      /* x not finite */
        ((hp >= 0x7ff00000) &&      /* p is NaN */
         (((hp - 0x7ff00000) | lp) != 0)))
        return (x * p) / (x * p);

    if (hp <= 0x7fdfffff)
        x = fmod(x, p + p); /* now x < 2p */
    if (((hx - hp) | (lx - lp)) == 0)
        return zero * x;
    x = fabs(x);
    p = fabs(p);
    if (hp < 0x00200000) {
        if (x + x > p) {
            x -= p;
            if (x + x >= p)
                x -= p;
        }
    } else {
        p_half = (double) 0.5 * p;
        if (x > p_half) {
            x -= p;
            if (x >= p_half)
                x -= p;
        }
    }
    GET_HIGH_WORD(hx, x);
    SET_HIGH_WORD(x, hx ^ sx);
    return x;
}
