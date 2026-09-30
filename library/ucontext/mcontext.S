/*
 * ucontext machine context core for clib4 (PowerPC 32-bit)
 *
 * Pure userspace implementation based on the libucontext project
 * (https://github.com/kaniini/libucontext): the upstream ppc port relies
 * on the Linux swapcontext syscall, which does not exist on AmigaOS 4,
 * so all state is saved/restored here directly.
 *
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
 */

#include "ppc-asm.h"
#include "ucontext_asm.h"

/* Offsets inside mcontext_t (r3 points to it) */
#define MC_GREG(n)  (MC_GREGS + ((n) * 4))
#define MC_FPR(n)   (MC_FPRS + ((n) * 8))

/*
 * Save the current machine state into the mcontext_t pointed by r3.
 * Clobbers r4. The saved NIP is the address of the instruction following
 * the call to the containing function, so that resuming the context
 * "returns" from __get_mcontext()/__swap_mcontext() a second time.
 * r3 is saved as 0 so the resumed context returns 0.
 */
.macro SAVE_MCONTEXT
    stw     0,  MC_GREG(REG_R0)(3)
    stw     1,  MC_GREG(REG_SP)(3)
    stw     2,  MC_GREG(REG_R2)(3)
    stw     4,  MC_GREG(4)(3)           /* r4 salvato prima di usarlo come scratch */
    stw     5,  MC_GREG(5)(3)
    stw     6,  MC_GREG(6)(3)
    stw     7,  MC_GREG(7)(3)
    stw     8,  MC_GREG(8)(3)
    stw     9,  MC_GREG(9)(3)
    stw     10, MC_GREG(10)(3)
    stw     11, MC_GREG(11)(3)
    stw     12, MC_GREG(12)(3)
    li      4,  0
    stw     4,  MC_GREG(REG_R3)(3)      /* return value on resume */
    stw     13, MC_GREG(13)(3)
    stw     14, MC_GREG(14)(3)
    stw     15, MC_GREG(15)(3)
    stw     16, MC_GREG(16)(3)
    stw     17, MC_GREG(17)(3)
    stw     18, MC_GREG(18)(3)
    stw     19, MC_GREG(19)(3)
    stw     20, MC_GREG(20)(3)
    stw     21, MC_GREG(21)(3)
    stw     22, MC_GREG(22)(3)
    stw     23, MC_GREG(23)(3)
    stw     24, MC_GREG(24)(3)
    stw     25, MC_GREG(25)(3)
    stw     26, MC_GREG(26)(3)
    stw     27, MC_GREG(27)(3)
    stw     28, MC_GREG(28)(3)
    stw     29, MC_GREG(29)(3)
    stw     30, MC_GREG(30)(3)
    stw     31, MC_GREG(REG_R31)(3)
    mflr    4
    stw     4,  MC_GREG(REG_NIP)(3)     /* resume at the return address */
    stw     4,  MC_GREG(REG_LNK)(3)
    mfcr    4
    stw     4,  MC_GREG(REG_CCR)(3)
    mfctr   4
    stw     4,  MC_GREG(REG_CTR)(3)
    mfxer   4
    stw     4,  MC_GREG(REG_XER)(3)
#if !defined(_SOFT_FLOAT) && !defined(__SPE__)
    stfd    14, MC_FPR(14)(3)
    stfd    15, MC_FPR(15)(3)
    stfd    16, MC_FPR(16)(3)
    stfd    17, MC_FPR(17)(3)
    stfd    18, MC_FPR(18)(3)
    stfd    19, MC_FPR(19)(3)
    stfd    20, MC_FPR(20)(3)
    stfd    21, MC_FPR(21)(3)
    stfd    22, MC_FPR(22)(3)
    stfd    23, MC_FPR(23)(3)
    stfd    24, MC_FPR(24)(3)
    stfd    25, MC_FPR(25)(3)
    stfd    26, MC_FPR(26)(3)
    stfd    27, MC_FPR(27)(3)
    stfd    28, MC_FPR(28)(3)
    stfd    29, MC_FPR(29)(3)
    stfd    30, MC_FPR(30)(3)
    stfd    31, MC_FPR(31)(3)
    mffs    0
    stfd    0,  MC_FPSCR(3)
#endif
.endm

/*
 * Restore the machine state from the mcontext_t pointed by r3 and jump
 * to the saved NIP. Never returns.
 */
.macro RESTORE_MCONTEXT
    lwz     4,  MC_GREG(REG_CCR)(3)
    mtcrf   0xff, 4
    lwz     4,  MC_GREG(REG_XER)(3)
    mtxer   4
    lwz     4,  MC_GREG(REG_CTR)(3)
    mtctr   4
    lwz     4,  MC_GREG(REG_LNK)(3)
    mtlr    4
#if !defined(_SOFT_FLOAT) && !defined(__SPE__)
    lfd     0,  MC_FPSCR(3)
    mtfsf   0xff, 0
    lfd     14, MC_FPR(14)(3)
    lfd     15, MC_FPR(15)(3)
    lfd     16, MC_FPR(16)(3)
    lfd     17, MC_FPR(17)(3)
    lfd     18, MC_FPR(18)(3)
    lfd     19, MC_FPR(19)(3)
    lfd     20, MC_FPR(20)(3)
    lfd     21, MC_FPR(21)(3)
    lfd     22, MC_FPR(22)(3)
    lfd     23, MC_FPR(23)(3)
    lfd     24, MC_FPR(24)(3)
    lfd     25, MC_FPR(25)(3)
    lfd     26, MC_FPR(26)(3)
    lfd     27, MC_FPR(27)(3)
    lfd     28, MC_FPR(28)(3)
    lfd     29, MC_FPR(29)(3)
    lfd     30, MC_FPR(30)(3)
    lfd     31, MC_FPR(31)(3)
#endif
    lwz     0,  MC_GREG(REG_R0)(3)
    lwz     1,  MC_GREG(REG_SP)(3)
    lwz     2,  MC_GREG(REG_R2)(3)
    lwz     5,  MC_GREG(5)(3)
    lwz     6,  MC_GREG(6)(3)
    lwz     7,  MC_GREG(7)(3)
    lwz     8,  MC_GREG(8)(3)
    lwz     9,  MC_GREG(9)(3)
    lwz     10, MC_GREG(10)(3)
    lwz     11, MC_GREG(11)(3)
    lwz     12, MC_GREG(12)(3)
    lwz     13, MC_GREG(13)(3)
    lwz     14, MC_GREG(14)(3)
    lwz     15, MC_GREG(15)(3)
    lwz     16, MC_GREG(16)(3)
    lwz     17, MC_GREG(17)(3)
    lwz     18, MC_GREG(18)(3)
    lwz     19, MC_GREG(19)(3)
    lwz     20, MC_GREG(20)(3)
    lwz     21, MC_GREG(21)(3)
    lwz     22, MC_GREG(22)(3)
    lwz     23, MC_GREG(23)(3)
    lwz     24, MC_GREG(24)(3)
    lwz     25, MC_GREG(25)(3)
    lwz     26, MC_GREG(26)(3)
    lwz     27, MC_GREG(27)(3)
    lwz     28, MC_GREG(28)(3)
    lwz     29, MC_GREG(29)(3)
    lwz     30, MC_GREG(30)(3)
    lwz     31, MC_GREG(REG_R31)(3)
    lwz     4,  MC_GREG(REG_NIP)(3)
    mtctr   4
    lwz     4,  MC_GREG(4)(3)      /* r4 ripristinato dopo l'uso come scratch */
    lwz     3,  MC_GREG(REG_R3)(3)
    bctr
.endm

/* void __get_mcontext(mcontext_t *mc) */
FUNC_START(__get_mcontext)
    SAVE_MCONTEXT
    blr
FUNC_END(__get_mcontext)

/* void __set_mcontext(const mcontext_t *mc) - does not return */
FUNC_START(__set_mcontext)
    RESTORE_MCONTEXT
FUNC_END(__set_mcontext)

/* void __swap_mcontext(mcontext_t *omc, const mcontext_t *mc)
 * Saves the current state into omc, then activates mc (no return).
 * When omc is activated later, execution resumes as if
 * __swap_mcontext() had just returned to its caller. */
FUNC_START(__swap_mcontext)
    mr      5, 4                        /* SAVE_MCONTEXT clobbers r4 */
    SAVE_MCONTEXT
    mr      3, 5
    RESTORE_MCONTEXT                    /* no return */
FUNC_END(__swap_mcontext)

/*
 * Trampoline reached when a function started by makecontext() returns.
 * makecontext() placed ucp->uc_link in r31 and the address of this
 * trampoline in the link register.
 */
FUNC_START(__start_context)
    mr.     3, 31                       /* r3 = uc_link */
    beq     1f
    bl      setcontext                  /* activate the linked context */
1:  li      3, 0
    bl      exit                        /* no linked context: exit(0) */
    /* not reached */
FUNC_END(__start_context)
