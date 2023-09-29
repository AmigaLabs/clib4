/*
 * $Id: math_s_ldexpl.c,v 1.7 2023-07-19 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
ldexpl(long double x, int n) {
    return scalbn(x, n);
}
