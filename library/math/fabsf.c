/*
 * $Id: math_fabsf.c,v 1.4 2021-01-31 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#ifndef __SPE__
float
fabsf(float number) {
    union ieee_single x;

    x.value = number;

    /* Knock off the sign bit. */
    x.raw[0] &= 0x7fffffff;

    return x.value;
}
#endif