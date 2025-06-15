/*
* $Id: math_s_roundevenf.c,v 1.0 2025-06-14 12:04:24 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#define BIAS 0x3ff
#define MANT_DIG 53
#define MAX_EXP (2 * BIAS + 1)

double
roundeven (double x) {
	uint64_t ix, ux;
	EXTRACT_WORD64 (ix, x);
	ux = ix & 0x7fffffffffffffffULL;
	int exponent = ux >> (MANT_DIG - 1);
	if (exponent >= BIAS + MANT_DIG - 1) {
		/* Integer, infinity or NaN.  */
		if (exponent == MAX_EXP)
		/* Infinity or NaN; quiet signaling NaNs.  */
			return x + x;
      	else
			return x;
    }
  	else if (exponent >= BIAS) {
		/* At least 1; not necessarily an integer.  Locate the bits with
		exponents 0 and -1 (when the unbiased exponent is 0, the bit
		with exponent 0 is implicit, but as the bias is odd it is OK
		to take it from the low bit of the exponent).  */
		int int_pos = (BIAS + MANT_DIG - 1) - exponent;
		int half_pos = int_pos - 1;
		uint64_t half_bit = 1ULL << half_pos;
		uint64_t int_bit = 1ULL << int_pos;
		if ((ix & (int_bit | (half_bit - 1))) != 0)
		/* Carry into the exponent works correctly.  No need to test whether HALF_BIT is set.  */
		ix += half_bit;
      	ix &= ~(int_bit - 1);
    }
  	else if (exponent == BIAS - 1 && ux > 0x3fe0000000000000ULL)
    	/* Interval (0.5, 1).  */
    	ix = (ix & 0x8000000000000000ULL) | 0x3ff0000000000000ULL;
  	else
    	/* Rounds to 0.  */
    	ix &= 0x8000000000000000ULL;
  	INSERT_WORD64 (x, ix);
  	return x;
}