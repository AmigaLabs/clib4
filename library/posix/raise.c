/*
 * $Id: signal_raise.c,v 1.10 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

/* This table holds pointers to all signal handlers configured at a time. */
signal_handler_t NOCOMMON __signal_handler_table[NSIG] = {
    SIG_DFL, /* SIGHUP */
    SIG_DFL, /* SIGINT */
    SIG_DFL, /* SIGQUIT */
    SIG_DFL, /* SIGILL */
    SIG_DFL, /* SIGTRAP */
    SIG_DFL, /* SIGABRT */
    SIG_DFL, /* SIGEMT */
    SIG_DFL, /* SIGFPE */
    SIG_DFL, /* SIGKILL */
    SIG_DFL, /* SIGBUS */
    SIG_DFL, /* SIGSEGV */
    SIG_DFL, /* SIGSYS */
    SIG_DFL, /* SIGPIPE */
    SIG_DFL, /* SIGQUIT */
    SIG_DFL, /* SIGALRM */
    SIG_DFL, /* SIGTERM */
    SIG_DFL, /* SIGURG */
    SIG_DFL, /* SIGSTOP */
    SIG_DFL, /* SIGTSTP */
    SIG_DFL, /* SIGCONT */
    SIG_DFL, /* SIGCHLD */
    SIG_DFL, /* SIGTTIN */
    SIG_DFL, /* SIGTTOU */
    SIG_DFL, /* SIGIO */
    SIG_DFL, /* SIGXCPU */
    SIG_DFL, /* SIGXFSZ */
    SIG_DFL, /* SIGVTALRM */
    SIG_DFL, /* SIGPROF */
    SIG_DFL, /* SIGWINCH */
    SIG_DFL, /* SIGLOST */
    SIG_DFL, /* SIGUSR1 */
    SIG_DFL /* SIGUSR2 */
};

/* This holds a mask of all signals whose delivery is currently blocked.
   The sigaddset(), sigblock(), sigprocmask() and sigsetmask() functions
   modify or query it. */
int NOCOMMON __signals_blocked;

int
raise(int sig) {
    static int local_signals_blocked;

    int result = ERROR;

    ENTER();
    SHOWVALUE(sig);

    /* This has to be a well-known and supported signal. */
    if (sig < SIGHUP || sig > NSIG) {
        SHOWMSG("unknown signal number");

        __set_errno(EINVAL);
        goto out;
    }

    /* Can we deliver the signal? */
    if (FLAG_IS_CLEAR(__signals_blocked, (1 << sig)) && FLAG_IS_CLEAR(local_signals_blocked, (1 << sig))) {
        signal_handler_t handler;

        /* Which handler is installed for this signal? */
        handler = __signal_handler_table[sig - SIGHUP];

        /* Should we ignore this signal? */
        if (handler != SIG_IGN) {
            /* Block delivery of this signal to prevent recursion. */
            SET_FLAG(local_signals_blocked, (1 << sig));

            /* The default behaviour is to drop into abort(), or do
               something very much like it. */
            if (handler == SIG_DFL) {
                SHOWMSG("this is the default handler");

                if (sig == SIGINT || sig == SIGTERM) {
                    char break_string[80];

                    /* Turn off ^C checking for good. */
                    __check_abort_enabled = FALSE;

                    Fault(ERROR_BREAK, NULL, break_string, (LONG)
                    sizeof(break_string));

                    __print_termination_message(break_string);

                    SHOWMSG("bye, bye...");
                }
                /* If we have a SIGALRM without associated handler don't call abort but exit directly */
                else if (sig != SIGALRM) {
                    /* Drop straight into abort(), which might call signal()
                       again but is otherwise guaranteed to eventually
                       land us in _exit(). */
                    abort();
                } else if (sig == SIGALRM) {
                    __print_termination_message("Alarm Clock");

                    /* Block SIGALRM signal from raise again */
                    sigblock(SIGALRM);

                    /* Since we got a signal we interrrupt every sleep function like nanosleep */
                    Signal((struct Task *) __global_clib2->self, SIGBREAKF_CTRL_E);
                }
            }
            else if (handler == SIG_ERR) {
                __set_errno(EINVAL);
                result = ERROR;
                goto out;
            }
            else {
                SHOWMSG("calling the handler");

                (*handler)(sig);

                if (sig == SIGINT)
                    SetSignal(0, SIGBREAKF_CTRL_C);

                SHOWMSG("done.");
            }

            /* Unblock signal delivery again. */
            CLEAR_FLAG(local_signals_blocked, (1 << sig));
        }
        else {
            if (sig == SIGINT) {
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
