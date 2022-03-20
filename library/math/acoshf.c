/*
 * $Id: math_acoshf.c,v 1.4 2022-03-13 12:04:23 clib2devs Exp $
 *

 *
 *
 * PowerPC math library based in part on work by Sun Microsystems
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 *
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float
        one = 1.0,
        ln2 = 6.9314718246e-01; /* 0x3f317218 */

float acoshf(float x) {
    float t;
    int32_t hx;
    GET_FLOAT_WORD(hx, x);
    if (hx < 0x3f800000) {        /* x < 1 */
        return (x - x) / (x - x);
    } else if (hx >= 0x4d800000) {    /* x > 2**28 */
        if (hx >= 0x7f800000) {    /* x is inf of NaN */
            return x + x;
        } else
            return logf(x) + ln2;    /* acosh(huge)=log(2x) */
    } else if (hx == 0x3f800000) {
        return 0.0;            /* acosh(1) = 0 */
    } else if (hx > 0x40000000) {    /* 2**28 > x > 2 */
        t = x * x;
        return logf((float) 2.0 * x - one / (x + sqrtf(t - one)));
    } else {            /* 1<x<2 */
        t = x - one;
        return log1pf(t + sqrtf((float) 2.0 * t + t * t));
    }
}
