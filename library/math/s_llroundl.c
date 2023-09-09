/*
 * $Id: math_s_llroundl.c,v 1.1 2023-07-19 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long long
llroundl(long double x) {
    return llround(x);
}
