/*
 * $Id: math_s_casinhf.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

float complex
casinhf(float complex z) {
    float complex w;

    w = -1.0f * I * casinf(z * I);
    return (w);
}
