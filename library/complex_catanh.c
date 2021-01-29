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
catanh(double complex z)
{
    double complex zp = catan(CMPLX(-cimag(z), creal(z)));
    return CMPLX(cimag(zp), -creal(zp));
}

/****************************************************************************/

#endif /* COMPLEX_SUPPORT */
