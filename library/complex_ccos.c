/*
 * Experimental: Not a part of clib2 from Olaf Barthel
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

/****************************************************************************/

#if defined(COMPLEX_SUPPORT)

/****************************************************************************/

double
complex ccos(double complex z)
{
    return ccosh(CMPLX(-cimag(z), creal(z)));
}

/****************************************************************************/

#endif /* COMPLEX_SUPPORT */
