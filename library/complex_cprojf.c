/*
 * Experimental: Not a part of clib2 from Olaf Barthel
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

/****************************************************************************/

#if defined(COMPLEX_SUPPORT)

/****************************************************************************/

float complex
cprojf(float complex z)
{
    return (isinf(crealf(z)) || isinf(cimagf(z))) ?
           CMPLXF(INFINITY, copysignf(0.0, crealf(z))) : z;
}

/****************************************************************************/

#endif /* COMPLEX_SUPPORT */
