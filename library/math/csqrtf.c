/*
 * $Id: complex_csqrtf.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
 *

 *
 *****************************************************************************
 *
 * Documentation and source code for this library, and the most recent library
 * build are available from <https://github.com/afxgroup/clib2>.
 *
 *****************************************************************************
 * Imported and modified for inclusion in clib2 2021/01/25
 * Ola Soder <rolfkopman@gmail.com>
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

// TODO - Check

float complex
csqrtf(float complex z) {
    float x = crealf(z), y = cimagf(z), r, t, scale;

    if (y == 0.0f) {
        if (x == 0.0f) {
            return CMPLXF(0.0f, 0.0f);
        }

        r = sqrtf(fabsf(x));
        return x < 0.0f ? CMPLXF(0.0f, r) : CMPLXF(r, 0.0f);
    }

    if (x == 0.0f) {
        r = sqrtf(0.5f * fabsf(y));
        return y > 0 ? CMPLXF(r, r) : CMPLXF(r, -r);
    }

    /* Rescale to avoid internal overflow or underflow.  */
    if ((fabsf(x) > 4.0f) || (fabsf(y) > 4.0f)) {
        x *= 0.25f;
        y *= 0.25f;
        scale = 2.0f;
    } else {
        x *= 6.7108864e7f; /* 2^26 */
        y *= 6.7108864e7f;
        scale = 1.220703125e-4f; /* 2^-13 */
    }

    r = cabsf(CMPLXF(x, y));

    if (x > 0) {
        t = sqrtf(0.5f * r + 0.5f * x);
        r = scale * fabsf((0.5f * y) / t);
        t *= scale;
    } else {
        r = sqrtf(0.5f * r - 0.5f * x);
        t = scale * fabsf((0.5f * y) / r);
        r *= scale;
    }

    return y < 0 ? CMPLXF(t, -r) : CMPLXF(t, r);

}
