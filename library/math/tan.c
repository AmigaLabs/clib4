/*
 * $Id: math_tan.c,v 1.7 2023-07-18 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

inline static double
__tan(double x) {
    double y[2], z = 0.0;
    int n, ix;

    /* High word of x. */
    GET_HIGH_WORD(ix, x);

    /* |x| ~< pi/4 */
    ix &= 0x7fffffff;
    if (ix <= 0x3fe921fb)
        return __kernel_tan(x, z, 1);

        /* tan(Inf or NaN) is NaN */
    else if (ix >= 0x7ff00000)
        return x - x; /* NaN */

        /* argument reduction needed */
    else {
        n = __rem_pio2(x, y);
        return __kernel_tan(y[0], y[1], 1 - ((n & 1) << 1)); /*   1 -- n even
							-1 -- n odd */
    }
}

double
tan(double x) {
    double result;

    result = __tan(x);

    return (result);
}
