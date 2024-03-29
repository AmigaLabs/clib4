/*
 * $Id: math_s_roundf.c,v 1.4 2023-07-19 12:04:24 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float
roundf(float x) {
    float t;

    if (!isfinite(x))
        return (x);

    if (x >= 0.0) {
        t = floorf(x);
        if (t - x <= -0.5)
            t += 1.0;
        return (t);
    } else {
        t = floorf(-x);
        if (t + x <= -0.5)
            t += 1.0;
        return (-t);
    }
}
