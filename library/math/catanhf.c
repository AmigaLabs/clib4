/*
 * $Id: complex_catabhf.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

float complex

catanhf(float complex z) {
    float complex zp = catanf(CMPLXF(-cimagf(z), crealf(z)));
    return CMPLXF(cimagf(zp), -crealf(zp));
}
