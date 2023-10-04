/*
 * $Id: stdlib_siglongjmp.c,v 1.0 2023-02-28 14:12:15 clib4devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

void siglongjmp(sigjmp_buf env, int val) {
    SHOWVALUE(val);

    if (env[0].__mask_was_saved)
        /* Restore the saved signal mask.  */
        (void) sigprocmask(SIG_SETMASK, &env[0].__saved_mask, (sigset_t *) NULL);

    /* Call the machine-dependent function to restore machine state.  */
    _longjmp(env, val ?: 1);
}
