/*
 * $Id: math_s_cacosf.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

float complex
cacosf(float complex z) {
    float complex w;

    w = casinf(z);
    w = ((float) M_PI_2 - crealf(w)) - cimagf(w) * I;
    return (w);
}
