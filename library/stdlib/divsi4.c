/*
 * $Id: stdlib_divsi4.c,v 1.3 2006-01-08 12:04:25 clib2devs Exp $
*/

__asm("							\n\
	.text						\n\
	.align 2					\n\
								\n\
	.globl __divsi4				\n\
								\n\
__divis4:						\n\
	divw  r3, r3, r4			\n\
	blr							\n\
");
