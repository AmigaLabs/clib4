/* Copyright (C) 1991-2023 Free Software Foundation, Inc.
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

/* Copied from setjmp/setjmp.h, powerpc/bits/setjmp.h and modified
   appropriately to keep backward compatible with setjmp without
   AltiVec/VMX support.

   This file is not exported and the interfaces are private to libc.  */

#ifndef	__NOVMX_SETJMP_H
#define	__NOVMX_SETJMP_H	1

/* The following definitions are needed by ASM implementations of the old (novmx) __longjmp/__setjmp functions.  */

#define JB_GPR1   0  /* Also known as the stack pointer */
#define JB_GPR2   1
#define JB_LR     2  /* The address we will return to */
#define JB_GPRS   3  /* GPRs 14 through 31 are saved, 18 in total.  */
#define JB_CR     21 /* Condition code registers.  */
#define JB_FPRS   22 /* FPRs 14 through 31 are saved, 18*2 words total.  */
#define JB_SIZE   (58 * 4)

#endif
