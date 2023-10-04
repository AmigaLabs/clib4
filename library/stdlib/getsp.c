/*
 * $Id: stdlib_getsp.c,v 1.3 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

__asm("							\n\
								\n\
	.text						\n\
	.align 2					\n\
								\n\
	.globl __get_sp				\n\
								\n\
__get_sp:						\n\
								\n\
	mr	r3, r1					\n\
	blr							\n\
								\n\
");
