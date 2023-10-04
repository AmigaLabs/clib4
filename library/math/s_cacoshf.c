/*
 * $Id: math_s_cacoshf.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
*/
#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

float complex
cacoshf(float complex z) {
    float complex w;

    w = I * cacosf (z);
    return (w);
}
