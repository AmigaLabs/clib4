/*
 * $Id: math_s_modfl.c,v 1.0 2023-07-19 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
modfl(long double x, long double *iptr) {
    double d;
    long double r = modf(x, &d);
    *iptr = d;

    return r;
}
