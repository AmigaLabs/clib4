/*
 * $Id: math_llrintl.c,v 1.0 2022-03-11 07:54:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long long int
llrintl(long double x) {
    return llrint(x);
}
