/*
 * $Id: math_s_cacos.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex
cacos(double complex z) {
    double complex w;

    double complex tmp0, tmp1;

    tmp0 = casin(z);
    tmp1 = M_PI_2 - creal(tmp0);
    w = tmp1 - (cimag(tmp0) * (double complex) (double complex) I);
    return w;
}
