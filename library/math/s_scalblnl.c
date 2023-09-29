/*
 * $Id: math_s_scalblnl.c,v 1.0 2023-07-19 12:04:24 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
scalblnl(long double x, long int n) {
    return scalbln(x, n);
}
