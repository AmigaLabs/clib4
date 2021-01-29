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
casinf(float complex z)
{
    float x = crealf(z), y = cimagf(z);
    float complex w = CMPLXF(1.0f - (x - y) * (x + y), -2.0f * x * y),
                  r = clogf(CMPLX(-y, x) + csqrtf(w));
    return CMPLXF(cimagf(r), -crealf(r));
}

/****************************************************************************/

#endif /* COMPLEX_SUPPORT */
