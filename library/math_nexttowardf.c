/*
 * Experimental: Not a part of clib2 from Olaf Barthel
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

/****************************************************************************/

#if defined(FLOATING_POINT_SUPPORT)

/****************************************************************************/

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024

/****************************************************************************/

float
nexttowardf(float x, long double y)
{
    return nextafterf(x, y);
}

/****************************************************************************/

#endif /* LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024 */

/****************************************************************************/

#endif /* FLOATING_POINT_SUPPORT */
