/*
 * $Id: math_s_nextafter.c,v 1.3 2023-07-19 12:04:24 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
nextafter(double x, double y) {
    volatile double t;
    int32_t hx, hy, ix, iy;
    uint32_t lx, ly;

    EXTRACT_WORDS(hx, lx, x);
    EXTRACT_WORDS(hy, ly, y);
    ix = hx & 0x7fffffff;        /* |x| */
    iy = hy & 0x7fffffff;        /* |y| */

    if (((ix >= 0x7ff00000) && ((ix - 0x7ff00000) | lx) != 0) ||   /* x is nan */
        ((iy >= 0x7ff00000) && ((iy - 0x7ff00000) | ly) != 0))     /* y is nan */
        return x + y;
    if (x == y) return y;        /* x=y, return y */
    if ((ix | lx) == 0) {            /* x == 0 */
        INSERT_WORDS(x, hy & 0x80000000, 1);    /* return +-minsubnormal */
        t = x * x;
        if (t == x) return t; else return x;    /* raise underflow flag */
    }
    if (hx >= 0) {                /* x > 0 */
        if (hx > hy || ((hx == hy) && (lx > ly))) {    /* x > y, x -= ulp */
            if (lx == 0) hx -= 1;
            lx -= 1;
        } else {                /* x < y, x += ulp */
            lx += 1;
            if (lx == 0) hx += 1;
        }
    } else {                /* x < 0 */
        if (hy >= 0 || hx > hy || ((hx == hy) && (lx > ly))) {/* x < y, x -= ulp */
            if (lx == 0) hx -= 1;
            lx -= 1;
        } else {                /* x > y, x += ulp */
            lx += 1;
            if (lx == 0) hx += 1;
        }
    }
    hy = hx & 0x7ff00000;
    if (hy >= 0x7ff00000) return x + x;    /* overflow  */
    if (hy < 0x00100000) {        /* underflow */
        t = x * x;
        if (t != x) {        /* raise underflow flag */
            INSERT_WORDS(y, hx, lx);
            return y;
        }
    }
    INSERT_WORDS(x, hx, lx);
    return x;
}
