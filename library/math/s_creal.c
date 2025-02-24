/*
 * $Id: math_s_creal.c,v 1.0 2023-07-19 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double
creal(double complex z) {
    return (REAL(z));
}