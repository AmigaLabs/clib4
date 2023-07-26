/*
 * $Id: math_s_isfinite.c,v 1.3 2023-07-19 12:04:23 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
__isfinite_float(float f) {
#ifdef __SPE__
    union ieee_single x;
	int result;

	x.value = f;

	if((x.raw[0] & 0x7f800000) == 0x7f800000 && (x.raw[0] & 0x007fffff) != 0)
		result = 0; /* Exponent = 255 and fraction != 0.0 -> not a number */
	else if ((x.raw[0] & 0x7fffffff) == 0x7f800000)
		result = 0; /* Exponent = 255 and fraction = 0.0 -> infinity */
	else
		result = 1;

	return(result);
#else
    union IEEEf2bits u;

    u.f = f;
    return (u.bits.exp != 255);
#endif
}

int
__isfinite_double(double d) {
#ifdef __SPE__
    union ieee_double x;
	int result;

	x.value = d;

	if(((x.raw[0] & 0x7ff00000) == 0x7ff00000) && ((x.raw[0] & 0x000fffff) != 0 || (x.raw[1] != 0)))
		result = 0; /* Exponent = 2047 and fraction != 0.0 -> not a number */
	else if (((x.raw[0] & 0x7fffffff) == 0x7ff00000) && (x.raw[1] == 0))
		result = 0; /* Exponent = 2047 and fraction = 0.0 -> infinity */
	else
		result = 1;

	return(result);
#else
    union IEEEd2bits u;

    u.d = d;
    return (u.bits.exp != 2047);
#endif
}

int
__isfinite_long_double(long double d) {
    return __isfinite_double(d);
}