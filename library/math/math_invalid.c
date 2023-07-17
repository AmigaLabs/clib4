/*
 * $Id: math_math_invalid.c,v 1.0 2023-07-16 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const long double VAL = _FLOAT64_MIN;

__float64
__math_invalid(__float64 x) {
    if (isnan(x))
        return x + x;
    x = VAL / VAL;
    __set_errno(EDOM);
    return x;
}