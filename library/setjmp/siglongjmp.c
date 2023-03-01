/*
 * $Id: stdlib_siglongjmp.c,v 1.0 2023-02-28 14:12:15 clib2devs Exp $
*/

#include <setjmp.h>

void siglongjmp(sigjmp_buf env, int val) {
    if (env[0].__mask_was_saved)
        /* Restore the saved signal mask.  */
        (void) sigprocmask(SIG_SETMASK, &env[0].__saved_mask, (sigset_t *) NULL);

    /* Call the machine-dependent function to restore machine state.  */
    longjmp(env, val ?: 1);
}