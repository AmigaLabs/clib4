/*
 * $Id: math_s_scalbln.c,v 1.4 2023-07-19 12:04:24 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
scalbln(double x, long n) {
    return scalbn(x, n > INT_MAX ? INT_MAX : n < INT_MIN ? INT_MIN : n);
}
