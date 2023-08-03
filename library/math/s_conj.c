/*
 * $Id: math_s_conj.c,v 1.2 2023-07-19 12:04:22 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex
conj(double complex z) {
    return CMPLX(creal(z), -cimag(z));
}
