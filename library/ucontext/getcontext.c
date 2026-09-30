/*
 * getcontext() for clib4 (PowerPC 32-bit)
 *
 * Userspace implementation based on libucontext.
 */

#include <ucontext.h>
#include <signal.h>
#include "ucontext_asm.h"

int
getcontext(ucontext_t *ucp) {
    ucp->uc_flags = 0;

    /* Save the current signal mask */
    sigprocmask(SIG_BLOCK, NULL, &ucp->uc_sigmask);

    /* Save the machine state. When this context is activated later via
     * setcontext() or swapcontext(), execution resumes right after this
     * call and this function returns 0 again. */
    __get_mcontext(&ucp->uc_mcontext);

    return 0;
}
