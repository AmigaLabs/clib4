/*
 * $Id: math_s_nearbyintl.c,v 1.4 2023-07-19 12:04:24 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
nearbyintl(long double x) {
    return nearbyint(x);
}
