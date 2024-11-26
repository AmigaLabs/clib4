/*
 * $Id: posix_sigwait.,v 1.0 2024-08-18 12:04:24 clib4devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

static void
ignore_signal(int sig) {
    struct _clib4 *__clib4 = __CLIB4;
    /* Remember the signal.  */
    __clib4->__was_sig = sig;
}

static int
do_sigwait(struct _clib4 *__clib4, const sigset_t *set, int *sig) {
    sigset_t tmp_mask;
    struct sigaction saved[NSIG];
    struct sigaction action;
    int save_errno;
    int this;
    /* Prepare set.  */
    sigfillset(&tmp_mask);
    /* Unblock all signals in the SET and register our nice handler.  */
    action.sa_handler = ignore_signal;
    action.sa_flags = 0;
    sigfillset(&action.sa_mask);    /* Block all signals for handler.  */
    /* Make sure we recognize error conditions by setting WAS_SIG to a
       value which does not describe a legal signal number.  */
    __clib4->__was_sig = -1;
    for (this = 1; this < NSIG; ++this)
        if (sigismember(set, this)) {
            /* Unblock this signal.  */
            sigdelset(&tmp_mask, this);
            /* Register temporary action handler.  */
            if (sigaction(this, &action, &saved[this]) != 0)
                goto restore_handler;
        }
    /* Now we can wait for signals.  */
    sigsuspend(&tmp_mask);
restore_handler:
    save_errno = errno;
    while (--this >= 1)
        if (sigismember(set, this))
            /* We ignore errors here since we must restore all handlers.  */
            sigaction(this, &saved[this], NULL);
    __set_errno(save_errno);
    /* Store the result and return.  */
    *sig = __clib4->__was_sig;
    return __clib4->__was_sig == -1 ? -1 : 0;
}

int
sigwait(const sigset_t *set, int *sig) {
    struct _clib4 *__clib4 = __CLIB4;
    /* __sigsuspend should be a cancellation point.  */
    return do_sigwait(__clib4, set, sig);
}