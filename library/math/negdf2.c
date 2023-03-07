/*
 * $Id: math_negdf2.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
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
	.globl	___negdf2

___negdf2:

	movel	a6,sp@-
	movel	"A4(_MathIeeeDoubBasBase)",a6
	moveml	sp@(8),d0/d1
	jsr		a6@(-60:W)
	movel	sp@+,a6
	rts

");

/****************************************************************************/

#endif /* IEEE_FLOATING_POINT_SUPPORT */
