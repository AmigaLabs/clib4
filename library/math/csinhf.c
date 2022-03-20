/*
 * $Id: complex_csinhf.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
 *

 *
 *****************************************************************************
 *
 * Documentation and source code for this library, and the most recent library
 * build are available from <https://github.com/afxgroup/clib2>.
 *
 *****************************************************************************
 * Imported and modified 2021/01/25 by Ola Soder <rolfkopman@gmail.com>
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

float complex
csinhf(float complex z) {
    float x = creal(z), y = cimag(z);
    return CMPLXF(sinhf(x) * cosf(y), coshf(x) * sinf(y));
}
