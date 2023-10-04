/*
 * $Id: math_lrintl.c,v 1.0 2023-07-19 12:04:24 clib4devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
nextafterl(long double x, long double y) {
    return nextafter(x, y);
}
