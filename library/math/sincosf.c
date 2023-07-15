/*
 * $Id: math_sincosf.c,v 1.1 2023-07-15 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

void
sincosf(float x, float *sinx, float *cosx) {
    int32_t ix;

    /* High word of x. */
    GET_FLOAT_WORD (ix, x);

    /* |x| ~< pi/4 */
    ix &= 0x7fffffff;
    if (ix <= 0x3f490fd8) {
        *sinx = __kernel_sinf(x, 0.0, 0);
        *cosx = __kernel_cosf(x, 0.0);
    } else if (ix >= 0x7f800000) {
        /* sin(Inf or NaN) is NaN */
        *sinx = *cosx = x - x;
    } else {
        /* Argument reduction needed.  */
        float y[2];
        int n;

        n = __rem_pio2f(x, y);
        switch (n & 3) {
            case 0:
                *sinx = __kernel_sinf(y[0], y[1], 1);
                *cosx = __kernel_cosf(y[0], y[1]);
                break;
            case 1:
                *sinx = __kernel_cosf(y[0], y[1]);
                *cosx = -__kernel_sinf(y[0], y[1], 1);
                break;
            case 2:
                *sinx = -__kernel_sinf(y[0], y[1], 1);
                *cosx = -__kernel_cosf(y[0], y[1]);
                break;
            default:
                *sinx = -__kernel_cosf(y[0], y[1]);
                *cosx = __kernel_sinf(y[0], y[1], 1);
                break;
        }
    }
}
