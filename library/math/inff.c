/*
 * $Id: math_inff.c,v 1.2 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

float
__inff(void)
{
	union ieee_single x;

	/* Exponent = 255 and fraction = 0.0 -> infinity */
	x.raw[0] = 0x7f800000;

	return (x.value);
}
