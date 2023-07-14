/*
 * $Id: math_fmaxf.c,v 1.4 2023-07-14 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float
fmaxf(float x, float y) {
    return (isgreaterequal(x, y) || isnan(y)) ? x : y;
}
