/*
 * ucontext.h for clib4 (PowerPC 32-bit)
 *
 * The implementation is based on the libucontext project
 * (https://github.com/kaniini/libucontext), adapted to work in pure
 * userspace (no kernel swapcontext syscall is available on AmigaOS 4).
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

#ifndef _UCONTEXT_H
#define _UCONTEXT_H

#include <features.h>
#include <signal.h>

__BEGIN_DECLS

/* Number of general purpose registers saved in gregset_t.
 * The layout follows the libucontext ppc port:
 *   0-31  -> r0-r31
 *   32    -> NIP (program counter)
 *   33    -> MSR (unused)
 *   34    -> original r3 (unused)
 *   35    -> CTR
 *   36    -> LNK (link register)
 *   37    -> XER
 *   38    -> CCR (condition register)
 *   39-47 -> unused (kept for layout compatibility)
 */
#define NGREG 48

typedef unsigned long greg_t;
typedef greg_t gregset_t[NGREG];

typedef struct {
    double __fpregs[32];
    double __fpscr;
} fpregset_t;

/* Machine-specific context (PowerPC 32-bit) */
typedef struct {
    gregset_t gregs;
    fpregset_t fpregs;
} mcontext_t;

typedef struct ucontext_t {
    unsigned long uc_flags;
    struct ucontext_t *uc_link;
    stack_t uc_stack;
    sigset_t uc_sigmask;
    mcontext_t uc_mcontext __attribute__((aligned(8)));
} ucontext_t;

/* Retrieve the current context, storing it in ucp. Returns 0. */
extern int getcontext(ucontext_t *__ucp);

/* Activate the context stored in ucp. Does not return on success. */
extern int setcontext(const ucontext_t *__ucp);

/* Save the current context in oucp and activate ucp. */
extern int swapcontext(ucontext_t *__oucp, const ucontext_t *__ucp);

/* Modify ucp (obtained via getcontext) so that when activated it calls
 * func with the given integer arguments. When func returns, the context
 * ucp->uc_link is activated; if uc_link is NULL the process exits. */
extern void makecontext(ucontext_t *__ucp, void (*__func)(void), int __argc, ...);

__END_DECLS

#endif /* _UCONTEXT_H */
