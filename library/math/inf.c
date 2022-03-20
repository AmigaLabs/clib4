/*
 * $Id: math_inf.c,v 1.2 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

double
__inf(void)
{
	union ieee_double x;

	/* Exponent = 2047 and fraction = 0.0 -> infinity */
	x.raw[0] = 0x7ff00000;
	x.raw[1] = 0x00000000;

	return (x.value);
}
