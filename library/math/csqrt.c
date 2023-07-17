/*
 * $Id: complex_csqrt.c,v 1.1 2023-07-18 11:33:52 clib2devs Exp $
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex
csqrt(double complex z) {
    double complex
    w;
    double x, y, r, t, scale;

    x = creal(z);
    y = cimag(z);

    if (y == (double) 0.0) {
        if (x == (double) 0.0) {
            w = (double) 0.0 + y * (double complex) I;
        } else {
            r = fabs(x);
            r = sqrt(r);
            if (x < (double) 0.0) {
                w = (double) 0.0 + r * (double complex) I;
            } else {
                w = r + y * (double complex) I;
            }
        }
        return w;
    }
    if (x == (double) 0.0) {
        r = fabs(y);
        r = sqrt((double) 0.5 * r);
        if (y > 0)
            w = r + r * (double complex) I;
        else
        w = r - r * (double complex) I;
        return w;
    }
    /* Rescale to avoid internal overflow or underflow.  */
    if ((fabs(x) > (double) 4.0) || (fabs(y) > (double) 4.0)) {
        x *= (double) 0.25;
        y *= (double) 0.25;
        scale = (double) 2.0;
    } else {
#if 1
        x *= (double) 1.8014398509481984e16;  /* 2^54 */
        y *= (double) 1.8014398509481984e16;
        scale = (double) 7.450580596923828125e-9; /* 2^-27 */
#else
        x *= (double) 4.0;
        y *= (double) 4.0;
        scale = (double) 0.5;
#endif
    }
    w = x + y * (double complex) I;
    r = cabs(w);
    if (x > 0) {
        t = sqrt((double) 0.5 * r + (double) 0.5 * x);
        r = scale * fabs(((double) 0.5 * y) / t);
        t *= scale;
    } else {
        r = sqrt((double) 0.5 * r - (double) 0.5 * x);
        t = scale * fabs(((double) 0.5 * y) / r);
        r *= scale;
    }
    if (y < 0)
        w = t - r * (double complex) I;
    else
    w = t + r * (double complex) I;
    return w;
}
