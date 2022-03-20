/*
 * $Id: complex_ccosf.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

float complex
ccosf(float complex z) {
    return ccoshf(CMPLXF(-cimagf(z), crealf(z)));
}
