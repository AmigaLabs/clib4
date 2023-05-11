/*
 * $Id: stdlib_setjmp.c,v 2.0 2010-10-20 13:50:17 clib2devs Exp $
 */
/* This is a simple version of setjmp and longjmp for the PowerPC.
   Ian Lance Taylor, Cygnus Support, 9 Feb 1994.
   Modified by Jeff Johnston, Red Hat Inc. 2 Oct 2001. */

#include "ppc-asm.h"

#if defined(__SPE__) && !defined(_SOFT_FLOAT)
#define _SOFT_FLOAT 1
#endif

#ifndef __SPE__
FUNC_START(setjmp_altivec)
    addi	3,3,15		# align Altivec to 16 byte boundary
    rlwinm	3,3,0,0,27

    stw		1,0(3)		# offset 0
    stwu	2,4(3)		# offset 4
    stwu	13,4(3)		# offset 8
    stwu	14,4(3)		# offset 12
    stwu	15,4(3)		# offset 16
    stwu	16,4(3)		# offset 20
    stwu	17,4(3)		# offset 24
    stwu	18,4(3)		# offset 28
    stwu	19,4(3)		# offset 32
    stwu	20,4(3)		# offset 36
    stwu	21,4(3)		# offset 40
    stwu	22,4(3)		# offset 44
    stwu	23,4(3)		# offset 48
    stwu	24,4(3)		# offset 52
    stwu	25,4(3)		# offset 56
    stwu	26,4(3)		# offset 60
    stwu	27,4(3)		# offset 64
    stwu	28,4(3)		# offset 68
    stwu	29,4(3)		# offset 72
    stwu	30,4(3)		# offset 76
    stwu	31,4(3)		# offset 80
    mflr	4
    stwu	4,4(3)		# offset 84
    mfcr	4
    stwu	4,4(3)		# offset 88
//						# one word pad to get floating point aligned on 8 byte boundary
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

/* save Altivec vrsave and vr20-vr31 registers */
    mfspr	4,256		# vrsave register
    stwu	4,16(3)		# offset 248
    addi	3,3,8
    stvx	20,0,3		# offset 256
    addi	3,3,16
    stvx	21,0,3		# offset 272
    addi	3,3,16
    stvx	22,0,3		# offset 288
    addi	3,3,16
    stvx	23,0,3		# offset 304
    addi	3,3,16
    stvx	24,0,3		# offset 320
    addi	3,3,16
    stvx	25,0,3		# offset 336
    addi	3,3,16
    stvx	26,0,3		# offset 352
    addi	3,3,16
    stvx	27,0,3		# offset 368
    addi	3,3,16
    stvx	28,0,3		# offset 384
    addi	3,3,16
    stvx	29,0,3		# offset 400
    addi	3,3,16
    stvx	30,0,3		# offset 416
    addi	3,3,16
    stvx	31,0,3		# offset 432

/* This requires a total of 240 + 8 + 8 + 12 * 16 == 448 bytes. */

    li		3,0
    blr
FUNC_END(setjmp_altivec)
#endif /* __SPE__ */


FUNC_START(setjmp)
#ifdef __SPE__
    mr		12,1
	rlwinm	1,1,0,0,27	# force 16-byte stack alignment
	stwu	12,-16(1)
#endif
    addi	3,3,7		# align to 8 byte boundary
    rlwinm	3,3,0,0,28

#ifdef __SPE__
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
#else
    stw		1,0(3)		# offset 0
    stwu	2,4(3)		# offset 4
    stwu	13,4(3)		# offset 8
    stwu	14,4(3)		# offset 12
    stwu	15,4(3)		# offset 16
    stwu	16,4(3)		# offset 20
    stwu	17,4(3)		# offset 24
    stwu	18,4(3)		# offset 28
    stwu	19,4(3)		# offset 32
    stwu	20,4(3)		# offset 36
    stwu	21,4(3)		# offset 40
    stwu	22,4(3)		# offset 44
    stwu	23,4(3)		# offset 48
    stwu	24,4(3)		# offset 52
    stwu	25,4(3)		# offset 56
    stwu	26,4(3)		# offset 60
    stwu	27,4(3)		# offset 64
    stwu	28,4(3)		# offset 68
    stwu	29,4(3)		# offset 72
    stwu	30,4(3)		# offset 76
    stwu	31,4(3)		# offset 80
#endif

/* From this point on until the end of this function, add 84
   to the offset shown if __SPE__.  This difference comes from
   the fact that we save 21 64-bit registers instead of 21
   32-bit registers above.  */
    mflr	5
    stwu	5,4(3)		# offset 84
    mfcr	4
    stwu	4,4(3)		# offset 88

#ifdef __SPE__
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
#endif

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

#ifdef __SPE__
    lwz		1,0(1)
	lwz		0,4(1)
	mtlr	0
#endif

    li		3,0
    blr
FUNC_END(setjmp)


