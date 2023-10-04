/*
 * $Id: math_s_round.c,v 1.4 2023-07-19 12:04:24 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
round(double x) {
    double t;
    uint32_t hx;

    GET_HIGH_WORD(hx, x);
    if ((hx & 0x7fffffff) == 0x7ff00000)
        return (x + x);

    if (!(hx & 0x80000000)) {
        t = floor(x);
        if (t - x <= -0.5)
            t += 1;
        return (t);
    } else {
        t = floor(-x);
        if (t + x <= -0.5)
            t += 1;
        return (-t);
    }
}
