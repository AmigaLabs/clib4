/*
 * $Id: signal_raise.c,v 1.10 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include "pthread/common.h"

static APTR
hook_function(struct Hook *hook, APTR userdata, struct Process *process) {
    uint32 pid = (uint32) userdata;
    (void) (hook);

    if (process->pr_ProcessID == pid) {
        return process;
    }

    return 0;
}

/* This table holds pointers to all signal handlers configured at a time. */
signal_handler_t __signal_handler_table[NSIG] = {
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

int
raise(int sig) {
    static int local_signals_blocked;

    int result = ERROR;
    ENTER();
    SHOWVALUE(sig);

    /* This has to be a well-known and supported signal. 0 is a valid signal*/
    if (sig < 0 || sig > NSIG) {
        SHOWMSG("unknown signal number");

        __set_errno(EINVAL);
        goto out;
    }

    /* If sig is 0 just set result to OK and go out */
    if (sig == 0) {
        result = OK;
        goto out;
    }

    /* Can we deliver the signal? */
    if ((FLAG_IS_CLEAR(__CLIB2->__signals_blocked, (1 << sig)) && FLAG_IS_CLEAR(local_signals_blocked, (1 << sig))) || sig == SIGKILL) {
        signal_handler_t handler;

        /* Which handler is installed for this signal? */
        handler = __signal_handler_table[sig - SIGHUP];

        /* Should we ignore this signal? */
        if (handler != SIG_IGN) {
            /* Block delivery of this signal to prevent recursion. */
            SHOWMSG("Blocking signal if it isn't a kill signal");
            if (sig != SIGINT && sig != SIGTERM && sig != SIGKILL)
                SET_FLAG(local_signals_blocked, (1 << sig));

            /* The default behaviour is to drop into abort(), or do
               something very much like it. */
            if (handler == SIG_DFL) {
                SHOWMSG("this is the default handler");

                if (sig == SIGINT || sig == SIGTERM || sig == SIGKILL) {
                    /* Check ig we have timer terminal running. If so let's kill it */
                    if (__CLIB2->tmr_real_task != NULL) {
                        struct Hook h = {{NULL, NULL}, (HOOKFUNC) hook_function, NULL, NULL};
                        int32 pid, process;

                        /* Block SIGALRM signal from raise */
                        sigblock(SIGALRM);
                        /* Get itimer process ID */
                        pid = __CLIB2->tmr_real_task->pr_ProcessID;

                        Forbid();
                        /* Scan for process */
                        process = ProcessScan(&h, (CONST_APTR) pid, 0);
                        /* If we find the process send a signal to kill it */
                        while (process > 0) {
                            /* Send a SIGBREAKF_CTRL_F signal until the timer task return to Wait state
                             * and can get the signal */
                            Signal((struct Task *) __CLIB2->tmr_real_task, SIGBREAKF_CTRL_F);
                            process = ProcessScan(&h, (CONST_APTR) pid, 0);
                            usleep(100);
                        }
                        Permit();
                        WaitForChildExit(pid);
                        __CLIB2->tmr_real_task = NULL;
                    }

                    char break_string[80];

                    /* Turn off ^C checking for good. */
                    __CLIB2->__check_abort_enabled = FALSE;

                    Fault(ERROR_BREAK, NULL, break_string, (LONG)
                    sizeof(break_string));

                    __print_termination_message(break_string);

                    SHOWMSG("bye, bye...");

                    /* Drop straight into abort(), which might call signal()
                       again but is otherwise guaranteed to eventually
                       land us in _exit(). */
                    abort();
                }
                /* If we have a SIGALRM without associated handler don't call abort but exit directly */
                if (sig == SIGALRM) {
                    __print_termination_message("Alarm Clock");

                    /* Block SIGALRM signal from raise again */
                    sigblock(SIGALRM);

                    /* Since we got a signal we interrrupt every sleep function like nanosleep */
                    Signal((struct Task *) __CLIB2->self, SIGBREAKF_CTRL_E);
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

                if (sig == SIGINT || sig == SIGTERM || sig == SIGKILL)
                    SetSignal(0, SIGBREAKF_CTRL_C);

                SHOWMSG("done.");
            }

            /* Unblock signal delivery again. */
            SHOWMSG("Unblocking signal");
            CLEAR_FLAG(local_signals_blocked, (1 << sig));
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
