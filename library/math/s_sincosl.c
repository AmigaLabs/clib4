/*
 * $Id: math_sincosl.c,v 1.0 2023-07-19 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

void
sincosl(double x, double *s, double *c) {
    *s = sinl(x);
    *c = cosl(x);
}
