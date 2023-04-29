/*
 * $Id: time_setitimer.c,v 1.0 2022-03-14 18:06:24 clib2devs Exp $
 *
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
            if (old_value != NULL) {
                /* Store the current time value in old_value */
                old_value->it_value.tv_sec = __CLIB2->tmr_time.it_value.tv_sec;
                old_value->it_value.tv_usec = __CLIB2->tmr_time.it_value.tv_usec;
                old_value->it_interval.tv_sec = __CLIB2->tmr_time.it_interval.tv_sec;
                old_value->it_interval.tv_usec = __CLIB2->tmr_time.it_interval.tv_usec;
            }

            __CLIB2->tmr_time.it_value.tv_sec = new_value->it_value.tv_sec;
            __CLIB2->tmr_time.it_value.tv_usec = new_value->it_value.tv_usec;
            __CLIB2->tmr_time.it_interval.tv_sec = new_value->it_interval.tv_sec;
            __CLIB2->tmr_time.it_interval.tv_usec = new_value->it_interval.tv_usec;

            /*  If we have a previous running task stop it */
            if (__CLIB2->tmr_real_task != NULL && (new_value->it_value.tv_sec != 0 || new_value->it_value.tv_usec != 0)) {
                Signal((struct Task *)__CLIB2->tmr_real_task, SIGBREAKF_CTRL_D);
            }
            else if (__CLIB2->tmr_real_task == NULL) {
                /* Create timer tasks */
                if ((new_value->it_value.tv_sec != 0 || new_value->it_value.tv_usec != 0)) {
                    __CLIB2->tmr_real_task = CreateNewProcTags(
                            NP_Name, "CLIB2_ITIMER_REAL_TASK",
                            NP_Entry, itimer_real_task,
                            NP_Child, TRUE,
                            NP_UserData, (int) &which,
                            NP_Output, DupFileHandle(Output()),
                            NP_CloseOutput, TRUE,
                            TAG_END);
                    if (!__CLIB2->tmr_real_task) {
                        __set_errno(EFAULT);
                        return -1;
                    }
                }
            }
            else {
                int pid = __CLIB2->tmr_real_task->pr_ProcessID;
                Signal((struct Task *)__CLIB2->tmr_real_task, SIGBREAKF_CTRL_F);
                WaitForChildExit(pid);
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