/*
 * $Id: stdlib_udivmodsi3.c,v 1.3 2006-01-08 12:04:26 clib2devs Exp $
*/

#if defined(__GNUC__)

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

	.globl	___umodsi3
	.globl	___UtilityBase

| D1.L = D0.L % D1.L unsigned

___umodsi3:

	moveml	sp@(4:W),d0/d1
	jbsr	___udivsi4
	movel	d1,d0
	rts

	.globl	___udivsi3

| D0.L = D0.L / D1.L unsigned

___udivsi3:
	moveml	sp@(4:W),d0/d1
___udivsi4:
	movel	"A4(___UtilityBase)",a0
	jmp		a0@(-156:W)

");

/****************************************************************************/

#endif /* __GNUC__ */
