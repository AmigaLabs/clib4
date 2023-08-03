/*
 * $Id: math_e_fmodl.c,v 1.0 2023-07-19 11:48:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
fmodl(long double x, long double y) {
    return fmod(x, y);
}
