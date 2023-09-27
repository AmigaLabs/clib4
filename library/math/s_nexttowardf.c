/*
 * $Id: math_s_nexttowardf.c,v 1.3 2023-07-19 12:04:24 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float
nexttowardf(float x, long double y) {
    return nextafterf(x, y);
}
