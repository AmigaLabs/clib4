/*
 * $Id: setjmpE500.c,v 1.0 2023-07-14 13:50:17 clib4devs Exp $
 */

/* This is a simple version of setjmp and longjmp for the PowerPC.
   Ian Lance Taylor, Cygnus Support, 9 Feb 1994.
   Modified by Jeff Johnston, Red Hat Inc. 2 Oct 2001. */

#include "ppc-asm.h"

#ifndef _SOFT_FLOAT
#define _SOFT_FLOAT 1
#endif

FUNC_START(setjmp_spe)
    mr		12,1
	rlwinm	1,1,0,0,27	# force 16-byte stack alignment
	stwu	12,-16(1)
    addi	3,3,7		# align to 8 byte boundary
    rlwinm	3,3,0,0,28

/* If we are E500, then save 64-bit registers.  */
	stw		12,4(3)		# offset 4
	evstdd	2,8(3)		# offset 8
	evstdd	13,16(3)	# offset 16
	evstdd	14,24(3)	# offset 24
	evstdd	15,32(3)	# offset 32
	evstdd	16,40(3)	# offset 40
	evstdd	17,48(3)	# offset 48
	evstdd	18,56(3)	# offset 56
	evstdd	19,64(3)	# offset 64
	evstdd	20,72(3)	# offset 72
	evstdd	21,80(3)	# offset 80
	evstdd	22,88(3)	# offset 88
	evstdd	23,96(3)	# offset 96
	evstdd	24,104(3)	# offset 104
	evstdd	25,112(3)	# offset 112
	evstdd	26,120(3)	# offset 120
	evstdd	27,128(3)	# offset 128
	evstdd	28,136(3)	# offset 136
	evstdd	29,144(3)	# offset 144
	evstdd	30,152(3)	# offset 152
	evstdd	31,160(3)	# offset 160

	/* Add 164 to r3 to account for the amount of data we just
	   stored.  Note that we are not adding 168 because the next
	   store instruction uses an offset of 4.  */
	addi	3,3,164

/* From this point on until the end of this function, add 84
   to the offset shown if __SPE__.  This difference comes from
   the fact that we save 21 64-bit registers instead of 21
   32-bit registers above.  */
    mflr	5
    stwu	5,4(3)		# offset 84
    mfcr	4
    stwu	4,4(3)		# offset 88

/* Save emulated FPU registers */
	stw		5,4(12)
	lis		9,IExec@ha
	mfspr	4,263		# get context (fpr at offset 168)
	li		6,144		# length
	addi	4,4,280		# source
	addi	5,3,8		# destination (one word pad for 8 byte alignment)
	lwz		3,IExec@l(9)
	lwz		7,124(3)	# CopyMem
	mtctr	7
	bctrl

#ifndef _SOFT_FLOAT
    stfdu	14,8(3)		# offset 96
    stfdu	15,8(3)		# offset 104
    stfdu	16,8(3)		# offset 112
    stfdu	17,8(3)		# offset 120
    stfdu	18,8(3)		# offset 128
    stfdu	19,8(3)		# offset 136
    stfdu	20,8(3)		# offset 144
    stfdu	21,8(3)		# offset 152
    stfdu	22,8(3)		# offset 160
    stfdu	23,8(3)		# offset 168
    stfdu	24,8(3)		# offset 176
    stfdu	25,8(3)		# offset 184
    stfdu	26,8(3)		# offset 192
    stfdu	27,8(3)		# offset 200
    stfdu	28,8(3)		# offset 208
    stfdu	29,8(3)		# offset 216
    stfdu	30,8(3)		# offset 224
    stfdu	31,8(3)		# offset 232
#endif

/* This requires a total of 21 * 4 + 18 * 8 + 4 + 4 + 4
   bytes == 60 * 4 bytes == 240 bytes.  */

    lwz		1,0(1)
	lwz		0,4(1)
	mtlr	0

    li		3,0
    blr
FUNC_END(setjmp_spe)
