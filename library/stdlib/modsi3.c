/*
 * $Id: stdlib_modsi3.c,v 1.3 2006-01-08 12:04:26 clib4devs Exp $
*/

__asm("							\n\
	.text						\n\
	.align 8					\n\
								\n\
	.globl __modsi3				\n\
								\n\
__modsi3: 						\n\
	stw    r0, 20(r1)			\n\
    divw   r0, r3, r4			\n\
    mullw  r0, r0, r4			\n\
    subf   r3, r0, r3			\n\
	blr							\n\
");
