/*
 * $Id: sigsetjmpE500.S,v 1.0 2023-07-13 13:50:17 clib2devs Exp $
 */

/*
 * C library -- sigsetjmp, siglongjmp
 *
 *      siglongjmp(a,v)
 * will generate a "return(v?v:1)" from the last call to
 *      sigsetjmp(a, savemask)
 * by restoring registers from the stack.
 * The previous signal state is restored if savemask is non-zero
 *
 * jmpbuf layout:
 *     +------------+
 *     |  savemask  |
 *     +------------+
 *     | sig state  |
 *     |            |
 *     | (4 words)  |
 *     |            |
 *     +------------+
 *     | saved regs |
 *     |    ...     |
 */

#include "ppc-asm.h"

FUNC_START(__sigsetjmp_spe)
	mr	r6,r3
	stw	r4,0(r3)
	or.	r7,r4,r4
	beq	1f
	li	r3,1			/* SIG_BLOCK, but doesn't matter */
					    /*            since set == NULL  */
	li	r4,0			/* set = NULL */
	mr	r5,r6			/* &oset */
	addi	r5,r5,4
	bl sigprocmask      /* sigprocmask(SIG_BLOCK, NULL, &oset)*/
	sc				    /* assume no error       XXX */
1:
	mflr	r11
	mfcr	r12
	mr	r10,r1
	mr	r9,r2

	/* FPRs */
	evstdd	r9,24+0*8(r6)
	evstdd	r10,24+1*8(r6)
	evstdd	r11,24+2*8(r6)
	evstdd	r12,24+3*8(r6)
	evstdd	r13,24+4*8(r6)
	evstdd	r14,24+5*8(r6)
	evstdd	r15,24+6*8(r6)
	evstdd	r16,24+7*8(r6)
	evstdd	r17,24+8*8(r6)
	evstdd	r18,24+9*8(r6)
	evstdd	r19,24+10*8(r6)
	evstdd	r20,24+11*8(r6)
	evstdd	r21,24+12*8(r6)
	evstdd	r22,24+13*8(r6)
	evstdd	r23,24+14*8(r6)
	evstdd	r24,24+15*8(r6)
	evstdd	r25,24+16*8(r6)
	evstdd	r26,24+17*8(r6)
	evstdd	r27,24+18*8(r6)
	evstdd	r28,24+19*8(r6)
	evstdd	r29,24+20*8(r6)
	evstdd	r30,24+21*8(r6)
	evstdd	r31,24+22*8(r6)

	li	r3,0
	blr
FUNC_END(__sigsetjmp_spe)

FUNC_START(siglongjmp_spe)

	/* FPRs */
	evldd	r9,24+0*8(r6)
	evldd	r10,24+1*8(r6)
	evldd	r11,24+2*8(r6)
	evldd	r12,24+3*8(r6)
	evldd	r13,24+4*8(r6)
	evldd	r14,24+5*8(r6)
	evldd	r15,24+6*8(r6)
	evldd	r16,24+7*8(r6)
	evldd	r17,24+8*8(r6)
	evldd	r18,24+9*8(r6)
	evldd	r19,24+10*8(r6)
	evldd	r20,24+11*8(r6)
	evldd	r21,24+12*8(r6)
	evldd	r22,24+13*8(r6)
	evldd	r23,24+14*8(r6)
	evldd	r24,24+15*8(r6)
	evldd	r25,24+16*8(r6)
	evldd	r26,24+17*8(r6)
	evldd	r27,24+18*8(r6)
	evldd	r28,24+19*8(r6)
	evldd	r29,24+20*8(r6)
	evldd	r30,24+21*8(r6)
	evldd	r31,24+22*8(r6)

	lwz	r7,0(r3)
	mr	r6,r4
	mtlr	r11
	mtcr	r12
	mr	r1,r10
	or.	r7,r7,r7
	beq	1f
	mr	r4,r3
	li	r3,3			/* SIG_SETMASK */
	addi	r4,r4,4	    /* &set */
	li	r5,0			/* oset = NULL */
	bl sigprocmask	    /* sigprocmask(SIG_SET, &set, NULL) */
	sc				    /* assume no error       XXX */
1:
	or.	r3,r6,r6
	bnelr
	li	r3,1
	blr
FUNC_END(siglongjmp_spe)

.section .note.GNU-stack,"",%progbits
