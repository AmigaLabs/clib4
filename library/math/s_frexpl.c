/*
 * $Id: math_s_frexpl.c,v 1.0 2023-07-19 11:48:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
frexpl(long double x, int *nptr) {
    return frexp(x, nptr);
}
