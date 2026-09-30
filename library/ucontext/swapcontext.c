/*
 * swapcontext() for clib4 (PowerPC 32-bit)
 *
 * Userspace implementation based on libucontext.
 */

#include <ucontext.h>
#include <signal.h>
#include "ucontext_asm.h"

int
swapcontext(ucontext_t *oucp, const ucontext_t *ucp) {
    /* Save the current signal mask into oucp, then switch to the
     * signal mask of the target context. When oucp is activated
     * later, its saved mask is restored by whoever activates it. */
    sigprocmask(SIG_BLOCK, NULL, &oucp->uc_sigmask);
    sigprocmask(SIG_SETMASK, &ucp->uc_sigmask, NULL);

    /* Save the current machine state into oucp and activate ucp.
     * When another context switches back to oucp, execution resumes
     * right after this call. */
    __swap_mcontext(&oucp->uc_mcontext, &ucp->uc_mcontext);

    return 0;
}
