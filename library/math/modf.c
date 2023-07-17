/*
 * $Id: math_modf.c,v 1.7 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double one = 1.0;

double
modf(double x, double *iptr) {
    int32_t i0, i1, j0;
    uint32_t i;
    EXTRACT_WORDS(i0, i1, x);
    j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;    /* exponent of x */
    if (j0 < 20) {            /* integer part in high x */
        if (j0 < 0) {            /* |x|<1 */
            INSERT_WORDS(*iptr, i0 & 0x80000000, 0);    /* *iptr = +-0 */
            return x;
        } else {
            i = (0x000fffff) >> j0;
            if (((i0 & i) | i1) == 0) {        /* x is integral */
                uint32_t high;
                *iptr = x;
                GET_HIGH_WORD(high, x);
                INSERT_WORDS(x, high & 0x80000000, 0);    /* return +-0 */
                return x;
            } else {
                INSERT_WORDS(*iptr, i0 & (~i), 0);
                return x - *iptr;
            }
        }
    } else if (j0 > 51) {        /* no fraction part */
        uint32_t high;
        if (j0 == 0x400) {        /* inf/NaN */
            *iptr = x;
            return (double) 0.0 / x;
        }
        *iptr = x * one;
        GET_HIGH_WORD(high, x);
        INSERT_WORDS(x, high & 0x80000000, 0);    /* return +-0 */
        return x;
    } else {            /* fraction part in low x */
        i = ((uint32_t)(0xffffffff)) >> (j0 - 20);
        if ((i1 & i) == 0) {        /* x is integral */
            uint32_t high;
            *iptr = x;
            GET_HIGH_WORD(high, x);
            INSERT_WORDS(x, high & 0x80000000, 0);    /* return +-0 */
            return x;
        } else {
            INSERT_WORDS(*iptr, i0, i1 & (~i));
            return x - *iptr;
        }
    }
}
