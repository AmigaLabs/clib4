/*
 * Internal definitions shared between the ucontext assembler core
 * (mcontext.S) and the C wrappers. Not installed.
 *
 * Based on the libucontext ppc port (register layout in defs.h).
 */

#ifndef _UCONTEXT_ASM_H
#define _UCONTEXT_ASM_H

/* Register indices inside gregset_t (libucontext ppc layout) */
#define REG_R0  0
#define REG_SP  1   /* r1 is the stack pointer */
#define REG_R2  2
#define REG_R3  3
#define REG_R31 31
#define REG_NIP 32  /* program counter */
#define REG_MSR 33
#define REG_CTR 35
#define REG_LNK 36  /* link register */
#define REG_XER 37
#define REG_CCR 38

/* Offsets inside ucontext_t (see library/include/ucontext.h).
 * These are verified with _Static_assert in makecontext.c. */
#define UC_FLAGS    0
#define UC_LINK     4
#define UC_STACK    8   /* stack_t: ss_sp @ 8, ss_flags @ 12, ss_size @ 16 */
#define UC_SIGMASK  20  /* sigset_t is an int on clib4 */
#define UC_MCONTEXT 24  /* 8-byte aligned */

/* Offsets inside mcontext_t */
#define MC_GREGS 0                  /* gregset_t: 48 * 4 bytes */
#define MC_FPRS  192                /* fpregset_t.__fpregs */
#define MC_FPSCR (192 + (32 * 8))   /* fpregset_t.__fpscr */

#ifndef __ASSEMBLER__

#include <ucontext.h>

/* Assembler core (mcontext.S) */
extern void __get_mcontext(mcontext_t *mc);
extern void __set_mcontext(const mcontext_t *mc);
extern void __swap_mcontext(mcontext_t *omc, const mcontext_t *mc);

/* Trampoline entered when a makecontext() function returns */
extern void __start_context(void);

#endif /* __ASSEMBLER__ */

#endif /* _UCONTEXT_ASM_H */
