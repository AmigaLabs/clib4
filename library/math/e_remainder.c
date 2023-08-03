/*
 * $Id: math_e_remainder.c,v 1.4 2023-07-19 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double zero = 0.0;

double
__ieee754_remainder(double x, double p) {
    int32_t hx, hp;
    uint32_t sx, lx, lp;
    double p_half;

    EXTRACT_WORDS(hx, lx, x);
    EXTRACT_WORDS(hp, lp, p);
    sx = hx & 0x80000000;
    hp &= 0x7fffffff;
    hx &= 0x7fffffff;

    /* purge off exception values */
    if ((hp | lp) == 0) return (x * p) / (x * p);    /* p = 0 */
    if ((hx >= 0x7ff00000) ||            /* x not finite */
        ((hp >= 0x7ff00000) &&            /* p is NaN */
         (((hp - 0x7ff00000) | lp) != 0)))
        return ((long double) x * p) / ((long double) x * p);


    if (hp <= 0x7fdfffff) x = __ieee754_fmod(x, p + p);    /* now x < 2p */
    if (((hx - hp) | (lx - lp)) == 0) return zero * x;
    x = fabs(x);
    p = fabs(p);
    if (hp < 0x00200000) {
        if (x + x > p) {
            x -= p;
            if (x + x >= p) x -= p;
        }
    } else {
        p_half = 0.5 * p;
        if (x > p_half) {
            x -= p;
            if (x >= p_half) x -= p;
        }
    }
    GET_HIGH_WORD(hx, x);
    if ((hx & 0x7fffffff) == 0) hx = 0;
    SET_HIGH_WORD(x, hx ^ sx);
    return x;
}
