/*
 * $Id: math_e_powl.c,v 1.0 2023-07-19 12:04:24 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
powl(long double x, long double y) {
    return pow(x, y);
}
