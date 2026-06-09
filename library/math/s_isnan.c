/*
 * $Id: math_s_isnan.c,v 1.1 2023-07-19 16:47:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

int
__isnan(double d) {
#ifndef __SPE__
    union IEEEd2bits u;

    u.d = d;
    return (u.bits.exp == 2047 && (u.bits.manl != 0 || u.bits.manh != 0));
#else
    union ieee_double x;

	x.value = d;

	/* Exponent = 2047 and fraction != 0.0 -> NaN */
	return (((x.raw[0] & 0x7ff00000) == 0x7ff00000) && ((x.raw[0] & 0x000fffff) != 0 || (x.raw[1] != 0)));
#endif
}

int
__isnanf(float f) {
    union IEEEf2bits u;

    u.f = f;
    return (u.bits.exp == 255 && u.bits.man != 0);
}

int
__isnanl(long double d) {
    return __isnan(d);
}