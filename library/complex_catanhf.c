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
catanhf(float complex z)
{
    float complex zp = catanf(CMPLXF(-cimagf(z), crealf(z)));
    return CMPLXF(cimagf(zp), -crealf(zp));
}

/****************************************************************************/

#endif /* COMPLEX_SUPPORT */
