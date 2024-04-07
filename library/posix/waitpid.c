/*
 * $Id: posix_waitpid.c,v 1.0 2024-04-07 12:04:27 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <sys/wait.h>

/*
The value of pid can be:

< -1    : meaning wait for any child process whose process group ID is equal to the absolute value of pid.
-1      : meaning wait for any child process.
0       : meaning wait for any child process whose process group ID is equal to that of the calling process.
> 0     : meaning wait for the child whose process ID is equal to the value of pid.
*/

static APTR
waitpid_hook_function(struct Hook *hook, APTR userdata, struct Process *process) {
    uint32 pid = (uint32) userdata;
    (void) (hook);

    if (process->pr_ProcessID == pid) {
        return process;
    }

    return 0;
}

pid_t
waitpid(pid_t pid, int *status, int options) {
    pid_t processId = -1;
    int gid;
    struct Hook h = {{NULL, NULL}, (HOOKFUNC) waitpid_hook_function, NULL, NULL};
    struct Process *process = NULL;
    int32 retval;

    *status = 0;

    if (pid < -1) {
        // We don't have group wait at moment
        gid = abs(pid);
    } else if (pid == -1) {
        if (options != WNOHANG) {
            /* If WNOHANG is not set wait for process closing */
            WaitForChildExit(0); // I hope this will not hang everything.. What if there are some pthreads or a setitmer process?
        }
        else {
            /* If WNOHANG is set, use process scan to find the process */
            Forbid();
            retval = ProcessScan(&h, (CONST_APTR) pid, 0);
            Permit();
            if (retval > 0) {
                // If WNOHANG was specified and one or more child(ren) specified by pid exist, but have not yet changed state, then 0 is returned
                processId = 0;
            }
        }
    } else if (pid == 0) {
        // We don't have group wait at moment
        GetOwnerInfoTags(
            OI_ProcessInput, (struct Process *) FindTask(NULL),
            OI_OwnerGID, &gid,
            TAG_DONE);
        // Check for error
    }
    else {
        /* Check for specific process id */
        if (options != WNOHANG) {
            /* If WNOHANG is not set wait for process closing */
            WaitForChildExit(pid);
        }
        else {
            /* If WNOHANG is set, use process scan to find the process */
            retval = CheckForChildExit(pid);
            if (retval) {
                processId = pid;
            }
        }
    }

    return processId;
}