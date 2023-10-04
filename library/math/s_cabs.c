/*
 * $Id: math_s_cabs.c,v 1.0 2021-01-30 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double
cabs(double complex z) {
    return hypot(creal(z), cimag(z));
}
