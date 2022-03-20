/*
 * $Id: math_floorf.c,v 1.3 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float huge = 1.0e30;

float floorf(float x)
{
	LONG i0, j_0;
	ULONG i;
	GET_FLOAT_WORD(i0, x);
	j_0 = ((i0 >> 23) & 0xff) - 0x7f;
	if (j_0 < 23)
	{
		if (j_0 < 0)
		{ /* raise inexact if x != 0 */
			if (huge + x > (float)0.0)
			{ /* return 0*sign(x) if |x|<1 */
				if (i0 >= 0)
				{
					i0 = 0;
				}
				else if ((i0 & 0x7fffffff) != 0)
				{
					i0 = 0xbf800000U;
				}
			}
		}
		else
		{
			i = (0x007fffff) >> j_0;
			if ((i0 & i) == 0)
				return x; /* x is integral */
			if (huge + x > (float)0.0)
			{ /* raise inexact flag */
				if (i0 < 0)
					i0 += (0x00800000) >> j_0;
				i0 &= (~i);
			}
		}
	}
	else
	{
		if (j_0 == 0x80)
			return x + x; /* inf or NaN */
		else
			return x; /* x is integral */
	}
	SET_FLOAT_WORD(x, i0);
	return x;
}
