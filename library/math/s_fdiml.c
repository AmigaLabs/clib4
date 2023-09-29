/*
 * $Id: math_s_fdiml.c,v 1.1 2023-07-19 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
fdiml(long double x, long double y) {
    if (isnan(x))
        return (x);
    if (isnan(y))
        return (y);
    return (x > y ? x - y : 0.0);
}
