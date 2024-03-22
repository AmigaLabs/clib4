/*
 * $Id: stdlib_udivsi3.c,v 1.3 2006-01-08 12:04:26 clib4devs Exp $
*/

__asm("							\n\
    .text						\n\
    .align 8					\n\
								\n\
    .globl __udivsi3			\n\
__udivsi3:						\n\
     divwu r3, r3, r4			\n\
     blr						\n\
");
