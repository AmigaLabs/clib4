/*
 * Experimental: Not a part of clib2 from Olaf Barthel
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024

/****************************************************************************/

double
nexttoward(double x, long double y)
{
    return nextafter(x, y);
}

/****************************************************************************/

#endif /* LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024 */
