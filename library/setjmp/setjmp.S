/*
 * $Id: stdlib_setjmp.c,v 2.0 2010-10-20 13:50:17 clib4devs Exp $
 */
/* This is a simple version of setjmp and longjmp for the PowerPC.
   Ian Lance Taylor, Cygnus Support, 9 Feb 1994.
   Modified by Jeff Johnston, Red Hat Inc. 2 Oct 2001. */

#include "ppc-asm.h"

FUNC_START(setjmp)
    addi	3,3,7		# align to 8 byte boundary
    rlwinm	3,3,0,0,28

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

/* From this point on until the end of this function, add 84
   to the offset shown if __SPE__.  This difference comes from
   the fact that we save 21 64-bit registers instead of 21
   32-bit registers above.  */
    mflr	5
    stwu	5,4(3)		# offset 84
    mfcr	4
    stwu	4,4(3)		# offset 88

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

    li		3,0
    blr
FUNC_END(setjmp)



