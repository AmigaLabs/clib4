/*
 * $Id: stdlib_divmodsi3.c,v 1.3 2006-01-08 12:04:25 clib2devs Exp $
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

	.globl	___modsi3

| D1.L = D0.L % D1.L signed

___modsi3:

	moveml	sp@(4:W),d0/d1
	jbsr	___divsi4
	movel	d1,d0
	rts

	.globl	___divsi3
	.globl	___UtilityBase

| D0.L = D0.L / D1.L signed

___divsi3:
	moveml	sp@(4:W),d0/d1
___divsi4:
	movel	"A4(___UtilityBase)",a0
	jmp		a0@(-150:W)

");

/****************************************************************************/

#endif /* __GNUC__ */
