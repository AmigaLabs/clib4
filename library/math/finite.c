/*
 * $Id: math_isfinite.c,v 1.0 2021-01-16 18:15:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

int
finite(double x) {
    int32 hx;
    GET_HIGH_WORD(hx, x);
    return (int) ((uint32)((hx & 0x7fffffff) - 0x7ff00000) >> 31);
}