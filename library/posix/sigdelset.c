/*
 * $Id: signal_sigdelset.c,v 1.0 2022-02-16 20:25:00 clib2devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

int
sigdelset(sigset_t *set, int sig) {
    if (sig >= NSIG || sig <= 0) {
        __set_errno(errno);
        return -1;
    }
    *set &= ~(1 << (sig - 1));
    return 0;
}