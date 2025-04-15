/*
 * $Id: signal_kill.c,v 1.8 2006-04-05 08:39:45 clib4devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

#include <unistd.h>

int
kill(pid_t pid, int signal_number) {
    struct Process *cli_process;
    int result = ERROR;

    ENTER();

    SHOWVALUE(pid);
    SHOWVALUE(signal_number);

    if (signal_number < 0 || signal_number >= NSIG) {
        __set_errno(EINVAL);
        return result;
    }

    if (pid > 0) {
        if (pid == getpid()) {
            SHOWMSG("This is a kill for our process");
            result = raise(signal_number);
            SHOWVALUE(result);
        } else {
            SHOWMSG("This is a kill for a different process. Search it");
            struct Hook h = {{NULL, NULL}, (HOOKFUNC) processscan_hook_function, NULL, NULL};

            Forbid();

            int32 process = ProcessScan(&h, (CONST_APTR) pid, 0);
            if (process > 0) {
                cli_process = (struct Process *) process;

                SHOWMSG("found the process");

                result = 0;

                if (signal_number == SIGTERM || signal_number == SIGINT || signal_number == SIGQUIT) {
                    Signal((struct Task *) cli_process, SIGBREAKF_CTRL_C);
                } else {
                    SHOWMSG("but won't shut it down");
                }
            } else {
                SHOWMSG("didn't find the process");

                __set_errno(ESRCH);
            }

            Permit();
        }
    }

    RETURN(result);
    return (result);
}
