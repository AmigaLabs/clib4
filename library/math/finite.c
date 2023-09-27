/*
 * $Id: math_isfinite.c,v 1.0 2021-01-16 18:15:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

int
finite(double number) {
#ifndef __SPE__
    int32 hx;
    GET_HIGH_WORD(hx, number);
    return (int) ((uint32)((hx & 0x7fffffff) - 0x7ff00000) >> 31);
#else
    union ieee_double x;
	int result;

	x.value = number;

	if(((x.raw[0] & 0x7ff00000) == 0x7ff00000) && ((x.raw[0] & 0x000fffff) != 0 || (x.raw[1] != 0)))
		result = 0; /* Exponent = 2047 and fraction != 0.0 -> not a number */
	else if (((x.raw[0] & 0x7fffffff) == 0x7ff00000) && (x.raw[1] == 0))
		result = 0; /* Exponent = 2047 and fraction = 0.0 -> infinity */
	else
		result = 1;
    //Printf("finite %ld\n", result);

	return(result);
#endif
}