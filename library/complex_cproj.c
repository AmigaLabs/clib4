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
cproj(double complex z)
{
   return (isinf(creal(z)) || isinf(cimag(z))) ?
          CMPLX(INFINITY, copysign(0.0, creal(z))) : z;
}

/****************************************************************************/

#endif /* COMPLEX_SUPPORT */
