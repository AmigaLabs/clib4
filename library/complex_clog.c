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
clog(double complex z)
{
    return CMPLX(log(cabs(z)), carg(z));
}

/****************************************************************************/

#endif /* COMPLEX_SUPPORT */
