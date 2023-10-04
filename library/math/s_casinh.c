/*
 * $Id: math_s_casinh.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex
casinh(double complex z) {
    double complex w;

    w = -1.0 * I * casin (z * I);
    return (w);;
}
