/*
 * $Id: math_s_signbit.c,v 2.0 2023-07-19 12:04:24 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#ifndef __SPE__
int
__signbit_double(double d) {
#ifndef __SPE__
    union IEEEd2bits u;

    u.d = d;
    return (u.bits.sign);
#else
    union ieee_double x;
	int result;

	x.value = d;

	result = ((x.raw[0] = 0x80000000) != 0);

	return(result);
#endif
}

int
__signbit_float(float f) {
#ifndef __SPE__
    union IEEEf2bits u;

    u.f = f;
    return (u.bits.sign);
#else
    union ieee_single x;
	int result;

	x.value = f;

	result = ((x.raw[0] = 0x80000000) != 0);

	return(result);
#endif
}

int
__signbit_long_double(long double e) {
    return __signbit_double(e);
}
#else
int
__signbit_double(double d) {
    return __builtin_signbit(d);
}

int
__signbit_float(float f) {
    return __builtin_signbitf(f);
}

int
__signbit_long_double(long double e) {
    return __builtin_signbitl(e);
}
#endif