/*
 * $Id: math_s_nexttoward.c,v 1.3 2023-07-19 12:04:24 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
nexttoward(double x, long double y) {
    return nextafter(x, y);
}
