/*
 * $Id: stdlib_mulsi3.c,v 1.3 2006-01-08 12:04:26 clib4devs Exp $
*/

__asm("							\n\
    .text						\n\
    .align 2					\n\
								\n\
     .globl __mulsi3			\n\
								\n\
__mulsi3:						\n\
     mullw  r3, r3, r4			\n\
     blr						\n\
");
