/*
 * $Id: math_s_fmaxl.c,v 1.1 2023-07-19 11:48:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
fmaxl(long double x, long double y) {
    return fmax(x, y);
}
