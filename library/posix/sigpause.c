/*
 * $Id: posix_sigpause.,v 1.0 2024-08-18 12:04:24 clib4devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

int
sigpause(int sig_or_mask, int is_sig) {
    sigset_t set;

    if (is_sig != 0) {
        /* The modern X/Open implementation is requested.  */
        if (sigprocmask(0, NULL, &set) < 0 || sigdelset(&set, sig_or_mask) < 0)
            return -1;
    }
    else
        return -1;

    /* Note the sigpause() is a cancellation point.  But since we call
       sigsuspend() which itself is a cancellation point we do not have
       to do anything here.  */
    return sigsuspend(&set);
}