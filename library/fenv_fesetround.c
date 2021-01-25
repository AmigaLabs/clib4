/*
 * Experimental: Not a part of clib2 from Olaf Barthel
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

/****************************************************************************/

#if defined(FLOATING_POINT_SUPPORT)

/****************************************************************************/

int
fesetround(int mode)
{
    // We always round towards the nearest number. It's not possible to set
    // rounding mode but we can pretend if the caller wants to set the mode
    // that we're using anyway. By doing so we also inform the caller that
    // it's not possible to set another mode.
    return mode == FE_TONEAREST ? 0 : -1;
}

/****************************************************************************/

#endif /* FLOATING_POINT_SUPPORT */
