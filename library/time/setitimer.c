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

/* Kill a specific timer by ThreadID */
static void killitimer_by_thread(struct _clib4 *__clib4, uint32 threadID) {
    struct TimerNode *node, *next;
    
    /* Scan the timer list for timers belonging to the specified thread */
    for (node = (struct TimerNode *)__clib4->tmr_real_list.mlh_Head;
         (next = (struct TimerNode *)node->tn_Node.mln_Succ) != NULL;
         node = next) {
        
        if (node->tn_ThreadID == threadID) {
            struct Hook h = {{NULL, NULL}, (HOOKFUNC) hook_function, NULL, NULL};
            int32 pid, process;
            
            pid = node->tn_Process->pr_ProcessID;
            /* Scan for process */
            process = ProcessScan(&h, (CONST_APTR) pid, 0);
            DebugPrintF("Scan for process %ld (%ld) of thread %lu..\n", process, pid, threadID);
            
            while (process > 0) {
                DebugPrintF("Waiting for process %ld to close..\n", pid);
                /* Send a SIGBREAKF_CTRL_F signal until the timer task return in Wait and can get the signal */
                Signal((struct Task *)node->tn_Process, SIGBREAKF_CTRL_F);
                process = ProcessScan(&h, (CONST_APTR) pid, 0);
                Delay(10);
            }
            
            DebugPrintF("Process closed.. Wait For Child\n");
            WaitForChildExit(pid);
            DebugPrintF("Done\n");
            
            /* Remove from list and free */
            Remove((struct Node *)&node->tn_Node);
            FreeVec(node);
			node = NULL;
        }
    }
}

/* Kill all timers (legacy function for compatibility) */
void killitimer(void) {
    struct _clib4 *__clib4 = __CLIB4;
    uint32 currentThreadID = (uint32)FindTask(NULL);
    killitimer_by_thread(__clib4, currentThreadID);
};

int
__setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;
    uint32 currentThreadID = (uint32)FindTask(NULL);
    struct TimerNode *node = NULL;
    struct TimerNode *existing = NULL;

    if (new_value == NULL) {
        __set_errno_r(__clib4, EFAULT);
        return -1;
    }

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

            /* Check if this thread already has a timer */
            for (node = (struct TimerNode *)__clib4->tmr_real_list.mlh_Head;
                 node->tn_Node.mln_Succ != NULL;
                 node = (struct TimerNode *)node->tn_Node.mln_Succ) {
                if (node->tn_ThreadID == currentThreadID) {
                    existing = node;
                    break;
                }
            }

            /*  If we have a previous running task for this thread, stop it */
            if (existing != NULL && (new_value->it_value.tv_sec != 0 || new_value->it_value.tv_usec != 0)) {
                Signal((struct Task *)existing->tn_Process, SIGBREAKF_CTRL_D);
            }
            else if (existing == NULL) {
                /* Create timer task */
                if (new_value->it_value.tv_sec != 0 || new_value->it_value.tv_usec != 0) {
                    struct Process *timerProc = CreateNewProcTags(
                            NP_Name,        "ITIMER_TASK",
                            NP_Start,       itimer_real_task,
                            NP_Child,       TRUE,
                            NP_UserData,    (int) which,
                            NP_Output,      DupFileHandle(Output()),
                            NP_CloseOutput, TRUE,
                            TAG_END);
                    if (!timerProc) {
                        __set_errno_r(__clib4, EFAULT);
                        return -1;
                    }

                    /* Create a new TimerNode and add to list */
                    struct TimerNode *newNode = AllocVec(sizeof(struct TimerNode), MEMF_ANY | MEMF_CLEAR);
                    if (!newNode) {
                        /* Kill the process we just created */
                        Signal((struct Task *)timerProc, SIGBREAKF_CTRL_F);
                        __set_errno_r(__clib4, ENOMEM);
                        return -1;
                    }

                    newNode->tn_Process = timerProc;
                    newNode->tn_ThreadID = currentThreadID;
                    AddTail((struct List *)&__clib4->tmr_real_list, (struct Node *)&newNode->tn_Node);
                }
            }
            else {
                SHOWMSG("Block SIGALRM signal from raise");
                /* Block SIGALRM signal from raise */
                sigblock(SIGALRM);
                /* Kill itimer for current thread */
                SHOWMSG("killitimer");
                killitimer_by_thread(__clib4, currentThreadID);
                SHOWMSG("Done");
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