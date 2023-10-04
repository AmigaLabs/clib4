/*
 * $Id: math_s_fdimf.c,v 1.4 2023-07-19 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float
fdimf(float x, float y) {
    if (isnan(x))
        return (x);
    if (isnan(y))
        return (y);
    return (x > y ? x - y : 0.0);
}
