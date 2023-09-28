/*
 * $Id: stdlib_udivsi4.c,v 1.3 2006-01-08 12:04:26 clib4devs Exp $
*/

__asm("							\n\
    .text						\n\
    .align 2					\n\
								\n\
    .globl __udivsi4			\n\
__udivsi4:						\n\
     divwu r3, r3, r4			\n\
     blr						\n\
");
