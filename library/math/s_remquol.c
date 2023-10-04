/*
 * $Id: math_s_remquol.c,v 1.0 2023-07-19 12:04:24 clib4devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
remquol(long double x, long double y, int *quo) {
    return remquo(x, y, quo);
}
