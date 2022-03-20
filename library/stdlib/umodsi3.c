/*
 * $Id: stdlib_umodsi3.c,v 1.3 2006-01-08 12:04:26 clib2devs Exp $
*/

__asm("							\n\
	.text						\n\
	.align 2					\n\
								\n\
	.globl __umodsi3			\n\
								\n\
__umodsi3:						\n\
	divwu	r0,r3,r4			\n\
 	mullw	r0,r0,r4			\n\
 	subf	r3,r0,r3			\n\
	blr							\n\
");
