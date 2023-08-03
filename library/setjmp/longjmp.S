/*
 * $Id: stdlib_longjmp.c,v 1.6 2010-10-20 13:50:17 clib2devs Exp $
 */

/* This is a simple version of setjmp and longjmp for the PowerPC.
   Ian Lance Taylor, Cygnus Support, 9 Feb 1994.
   Modified by Jeff Johnston, Red Hat Inc. 2 Oct 2001. */

#include "ppc-asm.h"

FUNC_START(longjmp)
    addi	3,3,7		# align to 8 byte boundary
    rlwinm	3,3,0,0,28

    lwz		1,0(3)		# offset 0
    lwzu	2,4(3)		# offset 4
    lwzu	13,4(3)		# offset 8
    lwzu	14,4(3)		# offset 12
    lwzu	15,4(3)		# offset 16
    lwzu	16,4(3)		# offset 20
    lwzu	17,4(3)		# offset 24
    lwzu	18,4(3)		# offset 28
    lwzu	19,4(3)		# offset 32
    lwzu	20,4(3)		# offset 36
    lwzu	21,4(3)		# offset 40
    lwzu	22,4(3)		# offset 44
    lwzu	23,4(3)		# offset 48
    lwzu	24,4(3)		# offset 52
    lwzu	25,4(3)		# offset 56
    lwzu	26,4(3)		# offset 60
    lwzu	27,4(3)		# offset 64
    lwzu	28,4(3)		# offset 68
    lwzu	29,4(3)		# offset 72
    lwzu	30,4(3)		# offset 76
    lwzu	31,4(3)		# offset 80

    lwzu	5,4(3)		# offset 84
    mtlr	5
    lwzu	5,4(3)		# offset 88
    mtcrf	255,5

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

    mr.		3,4
    bclr+	4,2
    li		3,1
    blr
FUNC_END(longjmp)
