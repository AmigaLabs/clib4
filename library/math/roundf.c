/*
 * $Id: math_roundf.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
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
