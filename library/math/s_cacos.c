/*
 * $Id: math_s_cacos.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex
cacos(double complex z) {
    double complex
    w;

    w = casin(z);
    w = (M_PI_2 - creal(w)) - cimag(w) * I;
    return (w);
}
