/*
 * $Id: complex_ctanhf.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
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
ctanhf(float complex z) {
    float x = crealf(z), y = cimagf(z), d = coshf(2.0f * x) + cosf(2.0f * y);
    return CMPLX(sinhf(2.0f * x) / d, sinf(2.0f * y) / d);
}
