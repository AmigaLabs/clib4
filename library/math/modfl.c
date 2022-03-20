/*
 * $Id: math_modfj.c,v 1.0 2022-03-12 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
modfl(long double x, long double *iptr)
{
    double d;
    long double r = modf(x, &d);
    *iptr = d;

    return r;
}
