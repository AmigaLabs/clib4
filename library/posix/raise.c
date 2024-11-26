/*
 * $Id: signal_raise.c,v 1.11 2023-09-12 12:04:24 clib4devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include "pthread/common.h"

int
raise(int sig) {
    ENTER();

    struct _clib4 *__clib4 = __CLIB4;
    int result = ERROR;
    SHOWVALUE(sig);

    /* This has to be a well-known and supported signal. 0 is a valid signal*/
    if (sig < 0 || sig > NSIG) {
        SHOWMSG("unknown signal number");

        __set_errno_r(__clib4, EINVAL);
        goto out;
    }

    /* If sig is 0 just set result to OK and go out */
    if (sig == 0) {
        result = OK;
        goto out;
    }

    /* Can we deliver the signal? */
    if ((FLAG_IS_CLEAR(__clib4->__signals_blocked, (1 << sig)) && FLAG_IS_CLEAR(__clib4->local_signals_blocked, (1 << sig))) || sig == SIGKILL) {
        signal_handler_t handler;

        /* Which handler is installed for this signal? */
        handler = __clib4->__signal_handler_table[sig - SIGHUP];

        /* Should we ignore this signal? */
        if (handler != SIG_IGN) {
            /* Block delivery of this signal to prevent recursion. */
            SHOWMSG("Blocking signal if it isn't a kill signal");
            if (sig != SIGINT && sig != SIGTERM && sig != SIGKILL)
                SET_FLAG(__clib4->local_signals_blocked, (1 << sig));

            /* The default behaviour is to drop into abort(), or do something very much like it. */
            if (handler == SIG_DFL) {
                SHOWMSG("this is the default handler");

                if (sig == SIGINT || sig == SIGTERM || sig == SIGKILL) {
                    __set_errno_r(__clib4, EINTR);
                    /* Check ig we have timer terminal running. If so let's kill it */
                    if (__clib4->tmr_real_task != NULL) {
                        /* Block SIGALRM signal from raise */
                        sigblock(SIGALRM);
                        /* Kill itimer */
                        killitimer();
                    }

                    char break_string[80];

                    /* Turn off ^C checking for good. */
                    __clib4->__check_abort_enabled = FALSE;

                    Fault(ERROR_BREAK, NULL, break_string, (LONG) sizeof(break_string));

                    __print_termination_message(break_string);

                    SHOWMSG("bye, bye...");

                    /* Drop straight into abort(), which might call signal()
                       again but is otherwise guaranteed to eventually
                       land us in _exit(). */
                    __abort();
                }
                /* If we have a SIGALRM without associated handler send the SIGBREAKF_CTRL_E signal */
                if (sig == SIGALRM) {
                    /* Block SIGALRM signal from raise again */
                    sigblock(SIGALRM);

                    /* Since we got a signal we interrrupt every sleep function like nanosleep */
                    Signal((struct Task *) __clib4->self, SIGBREAKF_CTRL_E);
                }
            }
            else if (handler == SIG_ERR) {
                __set_errno_r(__clib4, EINVAL);
                result = ERROR;
                goto out;
            }
            else {
                if (*handler != NULL) {
                    SHOWMSG("Calling the handler");
                    (*handler)(sig);

                    if (sig == SIGINT || sig == SIGTERM || sig == SIGKILL) {
                        __set_errno_r(__clib4, EINTR);
                        SetSignal(0, SIGBREAKF_CTRL_C);
                    }

                    SHOWMSG("done.");
                }
            }

            /* Unblock signal delivery again. */
            SHOWMSG("Unblocking signal");
            CLEAR_FLAG(__clib4->local_signals_blocked, (1 << sig));
        }
        else {
            if (sig == SIGINT || sig == SIGTERM || sig == SIGKILL) {
                // Reset the signal bit cleared by __check_abort()
                SetSignal(SIGBREAKF_CTRL_C, SIGBREAKF_CTRL_C);
            }
        }
    } else {
        SHOWMSG("that signal is blocked");
    }

    result = OK;

out:

    RETURN(result);
    return (result);
}
