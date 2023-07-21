/*
 * $Id: math_s_logb.c,v 1.9 2023-07-19 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double
        two54 = 1.80143985094819840000e+16;    /* 43500000 00000000 */

double
logb(double x) {
    int32_t lx, ix;
    EXTRACT_WORDS(ix, lx, x);
    ix &= 0x7fffffff;            /* high |x| */
    if ((ix | lx) == 0) return -1.0 / fabs(x);
    if (ix >= 0x7ff00000) return x * x;
    if (ix < 0x00100000) {
        x *= two54;         /* convert subnormal x to normal */
        GET_HIGH_WORD(ix, x);
        ix &= 0x7fffffff;
        return (double) ((ix >> 20) - 1023 - 54);
    } else
        return (double) ((ix >> 20) - 1023);
}
