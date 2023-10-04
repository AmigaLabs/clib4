/*
 * $Id: math_s_ccosf.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

static void
_cchshf(float xx, float *c, float *s) {
    float x, e, ei;

    x = xx;
    if (fabsf(x) <= 0.5f) {
        *c = coshf(x);
        *s = sinhf(x);
    } else {
        e = expf(x);
        ei = 0.5f / e;
        e = 0.5f * e;
        *s = e - ei;
        *c = e + ei;
    }
}

float complex
ccosf(float complex z) {
    float complex w;
    float ch, sh;

    _cchshf(cimagf(z), &ch, &sh);
    w = cosf(crealf(z)) * ch + (-sinf(crealf(z)) * sh) * I;
    return (w);
}