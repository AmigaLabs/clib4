/*
 * $Id: longjmpE500.c,v 1.0 2023-07-14 13:50:17 clib4devs Exp $
 */

/* This is a simple version of setjmp and longjmp for the PowerPC.
   Ian Lance Taylor, Cygnus Support, 9 Feb 1994.
   Modified by Jeff Johnston, Red Hat Inc. 2 Oct 2001. */

#include "ppc-asm.h"

#if defined(__SPE__) && !defined(_SOFT_FLOAT)
#define _SOFT_FLOAT 1
#endif

FUNC_START(longjmp_spe)
    mr		12,1
	rlwinm	1,1,0,0,27	# force 16-byte stack alignment
	stwu	12,-16(1)
    addi	3,3,7		# align to 8 byte boundary
    rlwinm	3,3,0,0,28

/* If we are E500, then restore 64-bit registers.  */
	lwz		12,4(3)		# offset 4
	evldd	2,8(3)		# offset 8
	evldd	13,16(3)	# offset 16
	evldd	14,24(3)	# offset 24
	evldd	15,32(3)	# offset 32
	evldd	16,40(3)	# offset 40
	evldd	17,48(3)	# offset 48
	evldd	18,56(3)	# offset 56
	evldd	19,64(3)	# offset 64
	evldd	20,72(3)	# offset 72
	evldd	21,80(3)	# offset 80
	evldd	22,88(3)	# offset 88
	evldd	23,96(3)	# offset 96
	evldd	24,104(3)	# offset 104
	evldd	25,112(3)	# offset 112
	evldd	26,120(3)	# offset 120
	evldd	27,128(3)	# offset 128
	evldd	28,136(3)	# offset 136
	evldd	29,144(3)	# offset 144
	evldd	30,152(3)	# offset 152
	evldd	31,160(3)	# offset 160

	/* Add 164 to r3 to account for the amount of data we just
	   loaded.  Note that we are not adding 168 because the next
	   load instruction uses an offset of 4.  */
	addi	3,3,164

/* From this point on until the end of this function, add 84
   to the offset shown if __SPE__.  This difference comes from
   the fact that we restore 21 64-bit registers instead of 21
   32-bit registers above.  */
    lwzu	5,4(3)		# offset 84
    stw		5,4(12)
    lwzu	5,4(3)		# offset 88
    mtcrf	255,5

/* Restore emulated FPU registers */
	stw		4,8(1)
	stw		12,12(1)
	lis		9,IExec@ha
	mfspr	5,263		# get context (fpr at offset 168)
	li		6,144		# length
	addi	4,3,8		# source (one word pad for 8 byte alignment)
	addi	5,5,280		# destination
	lwz		3,IExec@l(9)
	lwz		7,124(3)	# CopyMem
	mtctr	7
	bctrl
	lwz		4,8(1)
	lwz		12,12(1)

#ifndef _SOFT_FLOAT
    lfdu	14,8(3)         # offset 96
    lfdu	15,8(3)         # offset 104
    lfdu	16,8(3)         # offset 112
    lfdu	17,8(3)         # offset 120
    lfdu	18,8(3)         # offset 128
    lfdu	19,8(3)         # offset 136
    lfdu	20,8(3)         # offset 144
    lfdu	21,8(3)         # offset 152
    lfdu	22,8(3)         # offset 160
    lfdu	23,8(3)         # offset 168
    lfdu	24,8(3)         # offset 176
    lfdu	25,8(3)         # offset 184
    lfdu	26,8(3)         # offset 192
    lfdu	27,8(3)         # offset 200
    lfdu	28,8(3)         # offset 208
    lfdu	29,8(3)         # offset 216
    lfdu	30,8(3)         # offset 224
    lfdu	31,8(3)         # offset 232
#endif

    lwz		0,4(12)
	mr		1,12
	mtlr	0

    mr.		3,4
    bclr+	4,2
    li		3,1
    blr
FUNC_END(longjmp_spe)
