/*
 * $Id: signal_sigprocmask.c,v 1.6 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

int
sigprocmask(int how, const sigset_t *set, sigset_t *oset) {
    int result = ERROR;

    ENTER();

    SHOWVALUE(how);
    SHOWPOINTER(set);
    SHOWPOINTER(oset);

    if (how < SIG_BLOCK || how > SIG_SETMASK) {
        SHOWMSG("invalid operating mode");

        __set_errno(EINVAL);
        goto out;
    }

    if (oset != NULL)
        (*oset) = __CLIB2->__signals_blocked;

    if (set != NULL) {
        SHOWVALUE(*set);

        switch (how) {
            case SIG_BLOCK:

                SHOWMSG("SIG_BLOCK");

                __CLIB2->__signals_blocked |= (*set);
                break;

            case SIG_UNBLOCK:

                SHOWMSG("SIG_UNBLOCK");

                __CLIB2->__signals_blocked &= ~(*set);
                break;

            case SIG_SETMASK:

                SHOWMSG("SIG_SETMASK");

                __CLIB2->__signals_blocked = (*set);
                break;
        }
    }

    result = OK;

out:

    RETURN(result);
    return (result);
}
