/*
 * $Id: math_fminl.c,v 1.0 2022-03-11 11:48:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
fminl(long double x, long double y)
{
    return fmin(x, y);
}
