/*
 * $Id: math_fdimf.c,v 1.4 2023-07-14 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float
fdimf(float x, float y) {
    int clsx = fpclassify(x);
    int clsy = fpclassify(y);

    if (clsx == FP_NAN || clsy == FP_NAN
        || (y < 0 && clsx == FP_INFINITE && clsy == FP_INFINITE))
        /* Raise invalid flag.  */
        return x - y;

    if (x <= y)
        return 0.0f;

    float r = x - y;
    if (fpclassify(r) == FP_INFINITE)
        __set_errno(ERANGE);

    return r;
}
