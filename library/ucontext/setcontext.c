/*
 * setcontext() for clib4 (PowerPC 32-bit)
 *
 * Userspace implementation based on libucontext.
 */

#include <ucontext.h>
#include <signal.h>
#include "ucontext_asm.h"

int
setcontext(const ucontext_t *ucp) {
    /* Restore the signal mask of the target context */
    sigprocmask(SIG_SETMASK, &ucp->uc_sigmask, NULL);

    /* Restore the machine state. This does not return: execution
     * continues from the point where the context was saved (or from
     * the function set up by makecontext()). */
    __set_mcontext(&ucp->uc_mcontext);

    /* Not reached */
    return -1;
}
