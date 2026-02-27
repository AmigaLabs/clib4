/*
 * $Id: resource_getpriority.c,v 1.0 2025-04-13 14:37:27 clib4devs Exp $
*/

#include <sys/resource.h>

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

int
setpriority (int which, unsigned int who, int priority) {

    if (priority < PRIO_MIN || priority > PRIO_MAX) {
        __set_errno(EINVAL);
        return -1;
    }

    switch (which) {
        case PRIO_PROCESS:
        case PRIO_PGRP:
        case PRIO_USER: {
            struct Hook h = {{NULL, NULL}, (HOOKFUNC) processscan_hook_function, NULL, NULL};

            int32 process = ProcessScan(&h, (CONST_APTR) who, 0);
            if (process > 0) {
                struct Process *cli_process = (struct Process *) process;
                SetTaskPri((struct Task *) cli_process, priority);
                return 0;
            }
            else {
                __set_errno(ESRCH);
                return -1;
            }
            break;
        }
        default:
            __set_errno(EINVAL);
            return -1;
    }
}