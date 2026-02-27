/*
 * $Id: posix_sigsuspend.,v 1.0 2024-08-18 12:04:24 clib4devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

int
sigsuspend(const sigset_t *mask) {
    int result = ERROR;
    sigset_t omask;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWPOINTER(mask);

    if (mask == NULL) {
        SHOWMSG("mask pointer is NULL");

        __set_errno_r(__clib4, EINVAL);
        goto out;
    }

    if (sigprocmask(SIG_SETMASK, mask, &omask) < 0) {
        goto out;
    }

    SHOWMSG("Calling pause");
    pause();
    SHOWMSG("Exit from pause");
    result = errno;

    if (sigprocmask(SIG_SETMASK, &omask, (sigset_t *) NULL) < 0) {
        goto out;
    }

    __set_errno_r(__clib4, result);

out:

    SHOWVALUE(result);

    LEAVE();

    return -1;
}
