/*
 * $Id: math_s_logbf.c,v 1.4 2023-07-19 12:04:23 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float
        two25 = 3.355443200e+07;        /* 0x4c000000 */

float
logbf(float x) {
    int32_t ix;
    GET_FLOAT_WORD(ix, x);
    ix &= 0x7fffffff;            /* high |x| */
    if (ix == 0) return (float) -1.0 / fabsf(x);
    if (ix >= 0x7f800000) return x * x;
    if (ix < 0x00800000) {
        x *= two25;         /* convert subnormal x to normal */
        GET_FLOAT_WORD(ix, x);
        ix &= 0x7fffffff;
        return (float) ((ix >> 23) - 127 - 25);
    } else
        return (float) ((ix >> 23) - 127);
}
