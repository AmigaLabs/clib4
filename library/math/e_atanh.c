/*
 * $Id: math_e_atanh.c,v 1.5 2023-07-19 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double one = 1.0, huge = 1e300;
static const double zero = 0.0;

double
__ieee754_atanh(double x) {
    double t;
    int32_t hx, ix;
    uint32_t lx;
    EXTRACT_WORDS(hx, lx, x);
    ix = hx & 0x7fffffff;
    if ((ix | ((lx | (-lx)) >> 31)) > 0x3ff00000) /* |x|>1 */
        return (x - x) / (x - x);
    if (ix == 0x3ff00000)
        return x / zero;
    if (ix < 0x3e300000 && (huge + x) > zero) return x;    /* x<2**-28 */
    SET_HIGH_WORD(x, ix);
    if (ix < 0x3fe00000) {        /* x < 0.5 */
        t = x + x;
        t = 0.5 * log1p(t + t * x / (one - x));
    } else
        t = 0.5 * log1p((x + x) / (one - x));
    if (hx >= 0) return t; else return -t;
}
