/*
 * $Id: math_s_nexttowardl.c,v 1.3 2023-07-19 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
nexttowardl(long double x, long double y) {
    return nextafter(x, y);
}
