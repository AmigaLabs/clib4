/*
 * $Id: math_nedf2.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

/****************************************************************************/

#if defined(IEEE_FLOATING_POINT_SUPPORT) && defined(__GNUC__)

/****************************************************************************/

#if defined(SMALL_DATA)
#define A4(x) "a4@(" #x ":W)"
#elif defined(SMALL_DATA32)
#define A4(x) "a4@(" #x ":L)"
#else
#define A4(x) #x
#endif /* SMALL_DATA */

/****************************************************************************/

asm("

	.text
	.even

	.globl	_MathIeeeDoubBasBase
	.globl	___nedf2

___nedf2:

	moveml	d2/d3/a6,sp@-
	movel	"A4(_MathIeeeDoubBasBase)",a6
	moveml	sp@(16),d0/d1/d2/d3
	jsr		a6@(-42:W)
	moveml	sp@+,d2/d3/a6
	rts

");

/****************************************************************************/

#endif /* IEEE_FLOATING_POINT_SUPPORT */
