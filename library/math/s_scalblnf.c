/*
 * $Id: math_s_scalblnf.c,v 1.4 2023-07-19 12:04:24 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float
scalblnf(float x, long n) {
    return scalbnf(x, n > INT_MAX ? INT_MAX : n < INT_MIN ? INT_MIN : n);
}
