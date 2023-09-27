/*
 * $Id: _setjmpE500.c,v 1.0 2023-07-14 13:50:17 clib4devs Exp $
 */

#include "ppc-asm.h"

FUNC_START(_setjmp_spe)
	mflr	r11
	mfcr	r12
	evstdd	r1,24+0*8(r3)
	evstdd	r2,24+1*8(r3)
	evstdd	r11,24+2*8(r3)
	evstdd	r12,24+3*8(r3)
	evstdd	r13,24+4*8(r3)
	evstdd	r14,24+5*8(r3)
	evstdd	r15,24+6*8(r3)
	evstdd	r16,24+7*8(r3)
	evstdd	r17,24+8*8(r3)
	evstdd	r18,24+9*8(r3)
	evstdd	r19,24+10*8(r3)
	evstdd	r20,24+11*8(r3)
	evstdd	r21,24+12*8(r3)
	evstdd	r22,24+13*8(r3)
	evstdd	r23,24+14*8(r3)
	evstdd	r24,24+15*8(r3)
	evstdd	r25,24+16*8(r3)
	evstdd	r26,24+17*8(r3)
	evstdd	r27,24+18*8(r3)
	evstdd	r28,24+19*8(r3)
	evstdd	r29,24+20*8(r3)
	evstdd	r30,24+21*8(r3)
	evstdd	r31,24+22*8(r3)

	li	r3,0
	blr
FUNC_END(_setjmp_spe)

.section .note.GNU-stack,"",%progbits
