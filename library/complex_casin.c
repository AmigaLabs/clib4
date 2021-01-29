/*
 * Experimental: Not a part of clib2 from Olaf Barthel
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

/****************************************************************************/

#if defined(COMPLEX_SUPPORT)

double complex
casin(double complex z)
{
    double x = creal(z), y = cimag(z);
    double complex w = CMPLX(1.0 - (x - y) * (x + y), -2.0 * x * y),
                   r = clog(CMPLX(-y, x) + csqrt(w));
    return CMPLX(cimag(r), -creal(r));
}

/****************************************************************************/

#endif /* COMPLEX_SUPPORT */
