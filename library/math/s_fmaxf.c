/*
 * $Id: math_s_fmaxf.c,v 1.4 2023-07-19 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float
fmaxf(float x, float y) {
    union IEEEf2bits u[2];

    u[0].f = x;
    u[1].f = y;

    /* Check for NaNs to avoid raising spurious exceptions. */
    if (u[0].bits.exp == 255 && u[0].bits.man != 0)
        return (y);
    if (u[1].bits.exp == 255 && u[1].bits.man != 0)
        return (x);

    /* Handle comparisons of signed zeroes. */
    if (u[0].bits.sign != u[1].bits.sign)
        return (u[u[0].bits.sign].f);

    return (x > y ? x : y);
}
