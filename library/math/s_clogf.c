/*
 * $Id: math_s_clogf.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

float complex
clogf(float complex z) {
    float complex w;
    float p, rr, x, y;

    x = crealf(z);
    y = cimagf(z);
    rr = atan2f(y, x);
    p = cabsf(z);
    p = logf(p);
    w = p + rr * I;
    return (w);
}
