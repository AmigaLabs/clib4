/*
 * $Id: math_s_fmin.c,v 1.4 2023-07-19 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
fmin(double x, double y) {
    union IEEEd2bits u[2];

    u[0].d = x;
    u[1].d = y;

    /* Check for NaNs to avoid raising spurious exceptions. */
    if (u[0].bits.exp == 2047 && (u[0].bits.manh | u[0].bits.manl) != 0)
        return (y);
    if (u[1].bits.exp == 2047 && (u[1].bits.manh | u[1].bits.manl) != 0)
        return (x);

    /* Handle comparisons of signed zeroes. */
    if (u[0].bits.sign != u[1].bits.sign)
        return (u[u[1].bits.sign].d);

    return (x < y ? x : y);
}
