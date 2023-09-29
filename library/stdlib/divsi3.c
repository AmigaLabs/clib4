/*
 * $Id: stdlib_divsi3.c,v 1.3 2006-01-08 12:04:25 clib4devs Exp $
*/

__asm ("						\n\
	 .text						\n\
	 .align 2					\n\
								\n\
	 .globl __divsi3			\n\
								\n\
__divsi3:						\n\
     divw  r3, r3, r4			\n\
	 blr						\n\
");
