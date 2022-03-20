/*
 * $Id: complex_clog.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex
clog(double complex z) {
    return CMPLX(log(cabs(z)), carg(z));
}
