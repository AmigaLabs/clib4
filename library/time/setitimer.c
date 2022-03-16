/*
 * $Id: time_setitimer.c,v 1.0 2022-03-14 18:06:24 apalmate Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * PowerPC math library based in part on work by Sun Microsystems
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

int
setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value) {

    ENTER();

    if (which < ITIMER_REAL || which > ITIMER_PROF) {
        __set_errno(EINVAL);
        return -1;
    }

    if (new_value == NULL) {
        __set_errno(EFAULT);
        return -1;
    }

    switch (which) {
        case ITIMER_REAL:
            if (old_value != NULL && __global_clib2->tmr_time != NULL) {
                /* Store the current time value in old_value */
                old_value->it_value.tv_sec = __global_clib2->tmr_time->it_value.tv_sec;
                old_value->it_value.tv_usec = __global_clib2->tmr_time->it_value.tv_usec;
                old_value->it_interval.tv_sec = __global_clib2->tmr_time->it_interval.tv_sec;
                old_value->it_interval.tv_usec = __global_clib2->tmr_time->it_interval.tv_usec;
                Printf("1)old_value->it_value.tv_sec=%d\n",old_value->it_value.tv_sec);
            }
            ObtainSemaphore(&__global_clib2->__tmr_access_sem);
            __global_clib2->tmr_time = (struct itimerval *) new_value;
            ReleaseSemaphore(&__global_clib2->__tmr_access_sem);
            if (old_value != NULL && __global_clib2->tmr_time != NULL)
                Printf("2)old_value->it_value.tv_sec=%d\n",old_value->it_value.tv_sec);
            /*  If we have a previous running task stop it */
            if (__global_clib2->tmr_real_task != NULL && (new_value->it_value.tv_sec != 0 || new_value->it_value.tv_usec != 0)) {
                printf("new_value->it_value.tv_sec=%d\n",__global_clib2->tmr_time->it_value.tv_sec);
                printf("new_value->it_value.tv_usec=%d\n",__global_clib2->tmr_time->it_value.tv_usec);
                int pid = __global_clib2->tmr_real_task->pr_ProcessID;
                printf("Reset previous task with pid %d\n", pid);
                Signal((struct Task *)__global_clib2->tmr_real_task, SIGBREAKF_CTRL_D);
                printf("Done\n");
            }
            else if (__global_clib2->tmr_real_task == NULL) {
                /* Create timer tasks */
                if ((new_value->it_value.tv_sec != 0 || new_value->it_value.tv_usec != 0)) {
                    Printf("Create task()\n");
                    __global_clib2->tmr_real_task = CreateNewProcTags(
                            NP_Name, "CLIB2_ITIMER_REAL_TASK",
                            NP_Entry, itimer_real_task,
                            NP_Child, TRUE,
                            NP_UserData, (int) &which,
                            NP_Output, DupFileHandle(Output()),
                            NP_CloseOutput, TRUE,
                            TAG_END);
                    if (!__global_clib2->tmr_real_task) {
                        Printf("Error creating task\n");
                        __set_errno(EFAULT);
                        return -1;
                    }
                    Printf("Created()\n");
                }
            }
            else {
                int pid = __global_clib2->tmr_real_task->pr_ProcessID;
                printf("Killing child2 %d\n", pid);
                Signal((struct Task *)__global_clib2->tmr_real_task, SIGBREAKF_CTRL_E);
                printf("3Wait for child2 %d\n", pid);
                //WaitForChildExit(pid);
                Printf("3Previous timer stopped\n");
            }

            break;
        case ITIMER_VIRTUAL:
            __set_errno(ENOSYS);
            return -1;
            break;
        case ITIMER_PROF:
            __set_errno(ENOSYS);
            return -1;
            break;
        default:
            break;
    }

    return 0;
}