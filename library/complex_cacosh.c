/*
 * Experimental: Not a part of clib2 from Olaf Barthel
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

/****************************************************************************/

#if defined(COMPLEX_SUPPORT)

/****************************************************************************/

double complex
cacosh(double complex z)
{
    double complex zp = cacos(z);
    return signbit(cimag(z)) ? CMPLX(cimag(zp), -creal(zp))
                             : CMPLX(-cimag(zp), creal(zp));
}

/****************************************************************************/

#endif /* COMPLEX_SUPPORT */
