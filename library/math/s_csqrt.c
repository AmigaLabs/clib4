/*
 * $Id: math_s_csqrt.c,v 1.2 2023-07-19 11:33:52 clib2devs Exp $
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex
csqrt(double complex z) {
    double complex w;
    double x, y, r, t, scale;

    x = creal(z);
    y = cimag(z);

    if (y == 0.0) {
        if (x == 0.0) {
            w = 0.0 + y * (double complex) I;
        } else {
            r = fabs(x);
            r = sqrt(r);
            if (x < 0.0) {
                w = 0.0 + r * (double complex) I;
            } else {
                w = r + y * (double complex) I;
            }
        }
        return w;
    }
    if (x == 0.0) {
        r = fabs(y);
        r = sqrt(0.5 * r);
        if (y > 0)
            w = r + r * (double complex) I;
        else
        w = r - r * (double complex) I;
        return w;
    }
    /* Rescale to avoid internal overflow or underflow.  */
    if ((fabs(x) > 4.0) || (fabs(y) > 4.0)) {
        x *= 0.25;
        y *= 0.25;
        scale = 2.0;
    } else {
#if 1
        x *= 1.8014398509481984e16;  /* 2^54 */
        y *= 1.8014398509481984e16;
        scale = 7.450580596923828125e-9; /* 2^-27 */
#else
        x *= 4.0;
        y *= 4.0;
        scale = 0.5;
#endif
    }
    w = x + y * (double complex) I;
    r = cabs(w);
    if (x > 0) {
        t = sqrt(0.5 * r + 0.5 * x);
        r = scale * fabs((0.5 * y) / t);
        t *= scale;
    } else {
        r = sqrt(0.5 * r - 0.5 * x);
        t = scale * fabs((0.5 * y) / r);
        r *= scale;
    }
    if (y < 0)
        w = t - r * (double complex) I;
    else
    w = t + r * (double complex) I;
    return w;
}
