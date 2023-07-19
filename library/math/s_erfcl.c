/*
 * $Id: math_s_erfcl.c,v 1.0 2023-07-19 12:04:23 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
erfcl(long double x) {
    return erfc(x);
}
