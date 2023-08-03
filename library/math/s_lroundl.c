/*
 * $Id: math_s_lroundl.c,v 1.1 2023-07-19 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long
lroundl(long double x) {
    return lround(x);
}
