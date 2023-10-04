/*
 * $Id: math_s_catabhf.c,v 1.1 2023-07-119 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

float complex
catanhf(float complex z) {
    float complex w;

    w = -1.0f * I * catanf(z * I);
    return (w);
}
