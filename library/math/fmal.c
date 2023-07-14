/*
 * $Id: math_fmal.c,v 1.0 2022-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
fmal(long double x, long double y, long double z) {
    return (x * y) + z;
}
