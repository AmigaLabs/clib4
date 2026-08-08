/*
 * makecontext() for clib4 (PowerPC 32-bit)
 *
 * Hand-written in assembler because reading the variadic arguments in C
 * through va_arg was miscompiled by GCC with -fomit-frame-pointer (the
 * register save area was addressed relative to the wrong base register),
 * corrupting every variadic argument after the first.
 *
 * Based on libucontext (arch/ppc/makecontext.c):
 * Copyright (c) 2018 Ariadne Conill <ariadne@dereferenced.org>
 * Copyright (c) 2019 Bobby Bingham <koorogi@koorogi.info>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * This software is provided 'as is' and without any warranty, express or
 * implied.  In no event shall the authors be liable for any damages arising
 * from the use of this software.
 *
 * Arguments (SYSV ABI):
 *   r3 = ucp, r4 = func, r5 = argc, r6-r10 = args 1-5, stack = args 6+
 * Only scratch registers (r6-r11, r0) are clobbered, so no stack frame
 * is needed and the stack arguments stay at a fixed offset from r1.
 */

#include "ppc-asm.h"

/* ucontext_t field offsets (8-byte aligned mcontext):
 *   uc_link @ 4, uc_stack.ss_sp @ 8, uc_stack.ss_size @ 16
 * mcontext_t starts at offset 24; gregs[n] at 24 + n*4 */
#define MC_GREG(n) (24 + ((n) * 4))

#define GREG_SP  MC_GREG(1)     /* r1  */
#define GREG_R31 MC_GREG(31)
#define GREG_NIP MC_GREG(32)    /* program counter */
#define GREG_LNK MC_GREG(36)    /* link register */

FUNC_START(makecontext)
    /*
     * Salva SUBITO gli argomenti variadici r6-r10 nei gregs[3..7],
     * prima di usare r6 come scratch per il calcolo dello stack.
     * Gli argomenti oltre il 5o sono sullo stack del chiamante a 0(r1)+.
     * (Scriverli tutti è sicuro anche se argc è minore: i gregs oltre
     * argc non sono mai letti dalla funzione target.)
     */
    stw     6,MC_GREG(3)(3)     /* arg1 */
    stw     7,MC_GREG(4)(3)     /* arg2 */
    stw     8,MC_GREG(5)(3)     /* arg3 */
    stw     9,MC_GREG(6)(3)     /* arg4 */
    stw     10,MC_GREG(7)(3)    /* arg5 */
    lwz     10,0(1)             /* arg6 dallo stack del chiamante */
    stw     10,MC_GREG(8)(3)
    lwz     10,4(1)             /* arg7 */
    stw     10,MC_GREG(9)(3)
    lwz     10,8(1)             /* arg8 */
    stw     10,MC_GREG(10)(3)

    /* r6 = number of stack arguments = max(argc, 8) - 8 */
    cmpwi   0,5,8
    li      6,8
    ble-    0,1f
    mr      6,5
1:
    /* sp = align16(ss_sp + ss_size - (stack_args + 2) * 4) */
    lwz     9,8(3)              /* ss_sp */
    lwz     10,16(3)            /* ss_size */
    subfic  6,6,6               /* 6 - r6 (stack_args + 2, negated) */
    add     9,9,10              /* end of stack */
    slwi    6,6,2               /* byte offset (negative) */
    add     9,9,6
    rlwinm  9,9,0,0,27          /* align to 16 bytes */

    stw     4,GREG_NIP(3)       /* entry point = func */
    lis     10,__start_context@ha
    la      10,__start_context@l(10)
    stw     10,GREG_LNK(3)      /* return to __start_context */
    lwz     10,4(3)             /* uc_link */
    stw     10,GREG_R31(3)      /* trampoline expects it in r31 */
    stw     9,GREG_SP(3)        /* new stack pointer */
    li      10,0
    stw     10,0(9)             /* back chain */

    /* Remaining arguments (argc > 8) go on the new stack at sp[2], sp[3], ... */
    cmpwi   0,5,8
    blelr-  0
    li      6,0                 /* byte offset from caller's arg base */
    addi    7,9,8               /* destination: &sp[2] */
2:
    addi    8,6,12              /* source offset: 1(r1) + 12 + i*4 */
    lwzx    10,1,8
    stwu    10,4(7)
    addi    6,6,4
    addi    8,6,12              /* argument index (i+2) */
    cmpw    0,8,5
    blt+    0,2b
    blr
FUNC_END(makecontext)
