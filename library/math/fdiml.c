/*
 * $Id: math_fdiml.c,v 1.0 2022-03-10 12:04:23 clib2devs Exp $
*/


#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
fdiml(long double x, long double y)
{
    double result;

    if (isnan(x))
        return x;

    if (isnan(y))
        return y;

    if (x > y)
        result = x - y;
    else
        result = 0;

    return (result);
}
