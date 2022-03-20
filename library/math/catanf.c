/*
 * $Id: complex_catanf.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
 *

 *
 *****************************************************************************
 *
 * Documentation and source code for this library, and the most recent library
 * build are available from <https://github.com/afxgroup/clib2>.
 *
 *****************************************************************************
 * Imported and modified 2021/01/25 by Ola Soder <rolfkopman@gmail.com>
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

static float
_redupif(float x) {
    float t = x / (float) M_PI, dp1 = 3.14159265160560607910E0,
            dp2 = 1.98418714791870343106E-9, dp3 = 1.14423774522196636802E-17;
    long int i = t >= 0.0f ? t + 0.5f : t - 0.5f;

    t = i;
    return ((x - t * dp1) - t * dp2) - t * dp3;
}

float complex
catanf(float complex z) {
    float x = crealf(z), y = cimagf(z), x2 = x * x, a = 1.0f - x2 - (y * y),
            w = _redupif(0.5f * atan2f(2.0f * x, a)), t = y - 1.0f;

    a = x2 + t * t;
    t = y + 1.0f;
    a = (x2 + t * t) / a;
    return CMPLXF(w, 0.25f * logf(a));
}
