/*
 * $Id: math_s_catanhl.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

long double complex
catanhl(long double complex z) {
    long double complex w;

    w = -1.0L * I * catanl(z * I);
    return (w);
}
