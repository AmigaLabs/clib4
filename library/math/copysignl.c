/*
 * $Id: math_copysignl.c,v 1.1 2023-07-14 12:04:23 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
copysignl(long double x, long double y) {
    if (signbit (x) != signbit (y))
        x = -x;
    return x;
}
