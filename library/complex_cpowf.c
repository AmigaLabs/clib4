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
cpowf(float complex z, float complex c)
{
    return cexpf(c * clogf(z));
}

/****************************************************************************/

#endif /* COMPLEX_SUPPORT */
