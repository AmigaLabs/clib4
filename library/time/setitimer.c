/*
 * $Id: time_setitimer.c,v 1.0 2022-03-14 18:06:24 clib4devs Exp $
 *
 */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

static APTR
hook_function(struct Hook *hook, APTR userdata, struct Process *process) {
    uint32 pid = (uint32) userdata;
    (void) (hook);

    if (process->pr_ProcessID == pid) {
        return process;
    }

    return 0;
}

void killitimer(void) {
    struct _clib4 *__clib4 = __CLIB4;
    struct Hook h = {{NULL, NULL}, (HOOKFUNC) hook_function, NULL, NULL};
    int32 pid, process;
    pid = __clib4->tmr_real_task->pr_ProcessID;
    /* Scan for process */
    process = ProcessScan(&h, (CONST_APTR) pid, 0);
    while (process > 0) {
        /* Send a SIGBREAKF_CTRL_F signal until the timer task return in Wait and can get the signal */
        Signal((struct Task *) __clib4->tmr_real_task, SIGBREAKF_CTRL_F);
        process = ProcessScan(&h, (CONST_APTR) pid, 0);
        Delay(10);
    }
    WaitForChildExit(pid);
    __clib4->tmr_real_task = NULL;

};

int
__setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;
    struct itimer _itimer;

    if (new_value == NULL) {
        __set_errno_r(__clib4, EFAULT);
        return -1;
    }

    _itimer.which = which;

    switch (which) {
        case -1:
        case ITIMER_REAL:
            if (old_value != NULL) {
                /* Store the current time value in old_value */
                old_value->it_value.tv_sec = __clib4->tmr_time.it_value.tv_sec;
                old_value->it_value.tv_usec = __clib4->tmr_time.it_value.tv_usec;
                old_value->it_interval.tv_sec = __clib4->tmr_time.it_interval.tv_sec;
                old_value->it_interval.tv_usec = __clib4->tmr_time.it_interval.tv_usec;
            }

            __clib4->tmr_time.it_value.tv_sec = new_value->it_value.tv_sec;
            __clib4->tmr_time.it_value.tv_usec = new_value->it_value.tv_usec;
            __clib4->tmr_time.it_interval.tv_sec = new_value->it_interval.tv_sec;
            __clib4->tmr_time.it_interval.tv_usec = new_value->it_interval.tv_usec;

            /*  If we have a previous running task stop it */
            if (__clib4->tmr_real_task != NULL && (new_value->it_value.tv_sec != 0 || new_value->it_value.tv_usec != 0)) {
                Signal((struct Task *)__clib4->tmr_real_task, SIGBREAKF_CTRL_D);
            }
            else if (__clib4->tmr_real_task == NULL) {
                /* Create timer tasks */
                if (new_value->it_value.tv_sec != 0 || new_value->it_value.tv_usec != 0) {
                    __clib4->tmr_real_task = CreateNewProcTags(
                            NP_Name, "ITIMER_TASK",
                            NP_Entry, itimer_real_task,
                            NP_Child, TRUE,
                            NP_UserData, (struct _itimer *) &_itimer,
                            NP_Output, DupFileHandle(Output()),
                            NP_CloseOutput, TRUE,
                            TAG_END);
                    if (!__clib4->tmr_real_task) {
                        __set_errno_r(__clib4, EFAULT);
                        return -1;
                    }
                }
            }
            else {
                /* Block SIGALRM signal from raise */
                sigblock(SIGALRM);
                /* Kill itimer */
                killitimer();
            }

            break;
        case ITIMER_VIRTUAL:
            __set_errno_r(__clib4, ENOSYS);
            return -1;
            break;
        case ITIMER_PROF:
            __set_errno_r(__clib4, ENOSYS);
            return -1;
            break;
        default:
            break;
    }

    return 0;
}

int
setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value) {

    if (which < ITIMER_REAL || which > ITIMER_PROF) {
        __set_errno(EINVAL);
        return -1;
    }

    return __setitimer(which, new_value, old_value);
}