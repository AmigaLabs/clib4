/*
 * $Id: math_math_invalidf.c,v 1.0 2023-07-16 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float VAL = 0.0f;

float
__math_invalidf(float x) {
    if (isnan(x))
        return x + x;
    x = VAL / VAL;
    __set_errno(EDOM);
    return x;
}