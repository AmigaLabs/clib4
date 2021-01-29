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
complex cacos(double complex z)
{
    double complex zp = casin(z);
    return CMPLX(M_PI_2 - creal(zp), -cimag(zp));
}

/****************************************************************************/

#endif /* COMPLEX_SUPPORT */
