/*
 * $Id: math_s_ilogb.c,v 1.3 2023-07-19 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

int
ilogb(double x) {
    int32_t hx, lx, ix;

    EXTRACT_WORDS(hx, lx, x);
    hx &= 0x7fffffff;
    if (hx < 0x00100000) {
        if ((hx | lx) == 0)
            return FP_ILOGB0;
        else            /* subnormal x */
        if (hx == 0) {
            for (ix = -1043; lx > 0; lx <<= 1) ix -= 1;
        } else {
            for (ix = -1022, hx <<= 11; hx > 0; hx <<= 1) ix -= 1;
        }
        return ix;
    } else if (hx < 0x7ff00000) return (hx >> 20) - 1023;
    else if (hx > 0x7ff00000 || lx != 0) return FP_ILOGBNAN;
    else return INT_MAX;
}
