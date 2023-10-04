/*
 * $Id: _longjmpE500.c,v 1.0 2023-07-14 13:50:17 clib4devs Exp $
 */

#include "ppc-asm.h"

FUNC_START(_longjmp_spe)
    evldd	r1,24+0*8(r3)
    evldd	r2,24+1*8(r3)
    evldd	r11,24+2*8(r3)
    evldd	r12,24+3*8(r3)
    evldd	r13,24+4*8(r3)
    evldd	r14,24+5*8(r3)
    evldd	r15,24+6*8(r3)
    evldd	r16,24+7*8(r3)
    evldd	r17,24+8*8(r3)
    evldd	r18,24+9*8(r3)
    evldd	r19,24+10*8(r3)
    evldd	r20,24+11*8(r3)
    evldd	r21,24+12*8(r3)
    evldd	r22,24+13*8(r3)
    evldd	r23,24+14*8(r3)
    evldd	r24,24+15*8(r3)
    evldd	r25,24+16*8(r3)
    evldd	r26,24+17*8(r3)
    evldd	r27,24+18*8(r3)
    evldd	r28,24+19*8(r3)
    evldd	r29,24+20*8(r3)
    evldd	r30,24+21*8(r3)
    evldd	r31,24+22*8(r3)

    mtlr	r11
    mtcr	r12
    or.	r3,r4,r4
    bnelr
    li	r3,1
    blr
FUNC_END(_longjmp_spe)

.section .note.GNU-stack,"",%progbits
