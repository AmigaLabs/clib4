/*
 * $Id: math_s_fdim.c,v 1.4 2023-07-19 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
fdim(double x, double y) {
    if (isnan(x))
        return (x);
    if (isnan(y))
        return (y);
    return (x > y ? x - y : 0.0);
}
