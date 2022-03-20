/*
 * $Id: signal_checkabort.c,v 1.4 2006-04-05 08:39:45 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

static APTR hook_function(struct Hook *hook, APTR userdata, struct Process *process) {
    uint32 pid = (uint32) userdata;
    (void) (hook);

    if (process->pr_ProcessID == pid) {
        return process;
    }

    return 0;
}

void
__check_abort(void) {
    if (__check_abort_enabled && FLAG_IS_SET(SetSignal(0, __break_signal_mask), __break_signal_mask)) {
        if (__global_clib2->tmr_real_task != NULL) {
            struct Hook h = {{NULL, NULL}, (HOOKFUNC) hook_function, NULL, NULL};
            int32 pid, process;

            Forbid();
            /* Block SIGALRM signal from raise */
            sigblock(SIGALRM);
            /* Get itimer process ID */
            pid = __global_clib2->tmr_real_task->pr_ProcessID;
            /* Scan for process */
            process = ProcessScan(&h, (CONST_APTR) pid, 0);
            /* If we find the process send a signal to kill it */
            while (process > 0) {
                /* Send a SIGBREAKF_CTRL_F signal until the timer task return in Wait and can get the signal */
                Signal((struct Task *) __global_clib2->tmr_real_task, SIGBREAKF_CTRL_F);
                process = ProcessScan(&h, (CONST_APTR) pid, 0);
            }
            WaitForChildExit(pid);
            __global_clib2->tmr_real_task = NULL;
            Permit();
        }
        raise(SIGINT);
    }
}
