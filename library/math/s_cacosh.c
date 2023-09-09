/*
 * $Id: math_s_cacosh.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex
cacosh(double complex z) {
    double complex w;

    w = I * cacos (z);
    return (w);
}
