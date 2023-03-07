/* setjmp for PowerPC.
   Copyright (C) 1995-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include "../cpu/4xx/ppc4xx.inc"
#include "stap-probe.h"

#define _ASM
#ifdef __NO_VMX__
/* The following definitions are needed by ASM implementations of the old (novmx) __longjmp/__setjmp functions.  */
# define JB_GPR1   0  /* Also known as the stack pointer */
# define JB_GPR2   1
# define JB_LR     2  /* The address we will return to */
# define JB_GPRS   3  /* GPRs 14 through 31 are saved, 18 in total.  */
# define JB_CR     21 /* Condition code registers.  */
# define JB_FPRS   22 /* FPRs 14 through 31 are saved, 18*2 words total.  */
# define JB_SIZE   (58 * 4)
#else
# define JB_GPR1   0  /* Also known as the stack pointer */
# define JB_GPR2   1
# define JB_LR     2  /* The address we will return to */
# define JB_GPRS   3  /* GPRs 14 through 31 are saved, 18 in total.  */
# define JB_CR     21 /* Condition code registers.  */
# define JB_FPRS   22 /* FPRs 14 through 31 are saved, 18*2 words total.  */
# define JB_SIZE   ((64 + (12 * 4)) * 4)
# define JB_VRSAVE 62
# define JB_VRS    64
#endif

function_prolog(_setjmp)
    li r4,0			/* Set second argument to 0.  */
    b __sigsetjmp@local
function_epilog(_setjmp)

function_prolog(__sigsetjmp_symbol)
	stw  r1,(JB_GPR1*4)(3)
	mflr r0
	/* setjmp probe expects longjmp first argument (4@3), second argument
	   (-4@4), and target address (4@0), respectively.  */
	LIBC_PROBE (setjmp, 3, 4@3, -4@4, 4@0)
	stw  r14,((JB_GPRS+14-14)*4)(3)
	stw  r0,(JB_LR*4)(3)
	stw  r15,((JB_GPRS+15-14)*4)(3)
	mfcr r0
	stw  r16,((JB_GPRS+16-14)*4)(3)
	stw  r0,(JB_CR*4)(3)
	stw  r17,((JB_GPRS+17-14)*4)(3)
	stw  r18,((JB_GPRS+18-14)*4)(3)
	stw  r19,((JB_GPRS+19-14)*4)(3)
	stw  r20,((JB_GPRS+20-14)*4)(3)
	stw  r21,((JB_GPRS+21-14)*4)(3)
	stw  r22,((JB_GPRS+22-14)*4)(3)
	stw  r23,((JB_GPRS+23-14)*4)(3)
	stw  r24,((JB_GPRS+24-14)*4)(3)
	stw  r25,((JB_GPRS+25-14)*4)(3)
	stw  r26,((JB_GPRS+26-14)*4)(3)
	stw  r27,((JB_GPRS+27-14)*4)(3)
	stw  r28,((JB_GPRS+28-14)*4)(3)
	stw  r29,((JB_GPRS+29-14)*4)(3)
	stw  r30,((JB_GPRS+30-14)*4)(3)
	stw  r31,((JB_GPRS+31-14)*4)(3)
	b __sigjmp_save_symbol@local
function_epilog(__sigsetjmp_symbol)
