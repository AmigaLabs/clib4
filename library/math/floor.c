/*
 * $Id: math_floor.c,v 1.6 2021-01-31 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double huge = 1.0e300;

INLINE STATIC double
__floor(double x)
{
	int i0, i1, j0;
	unsigned int i, j;

	EXTRACT_WORDS(i0, i1, x);

	j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;

	if (j0 < 20)
	{
		if (j0 < 0)
		{
			if (huge + x > 0.0)
			{
				if (i0 >= 0)
				{
					i0 = i1 = 0;
				}
				else if (((i0 & 0x7fffffff) | i1) != 0)
				{
					i0 = 0xbff00000;
					i1 = 0;
				}
			}
		}
		else
		{
			i = (0x000fffff) >> j0;

			if (((i0 & i) | i1) == 0)
				return x;

			if (huge + x > 0.0)
			{
				if (i0 < 0)
					i0 += (0x00100000) >> j0;
				i0 &= (~i);
				i1 = 0;
			}
		}
	}
	else if (j0 > 51)
	{
		if (j0 == 0x400)
			return x + x;
		else
			return x;
	}
	else
	{
		i = ((unsigned int)(0xffffffff)) >> (j0 - 20);

		if ((i1 & i) == 0)
			return x;

		if (huge + x > 0.0)
		{
			if (i0 < 0)
			{
				if (j0 == 20)
					i0 += 1;
				else
				{
					j = i1 + (1 << (52 - j0));
					if (j < i1)
						i0 += 1;
					i1 = j;
				}
			}
			i1 &= (~i);
		}
	}

	INSERT_WORDS(x, i0, i1);
	return x;
}

double
floor(double x)
{
	double result;

	result = __floor(x);

	return (result);
}
