/*
 * $Id: signal_sigismember.c,v 1.0 2022-02-16 12:04:24 clib2devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

int
sigismember(const sigset_t *set, int sig) {
    ENTER();

    if (sig < 0 || sig > NSIG - 1) {
        SHOWMSG("invalid signal");

        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    if (*set & sigmask(sig)) {
        RETURN(1);
        return 1;
    }
    else {
        RETURN(0);
        return 0;
    }
}