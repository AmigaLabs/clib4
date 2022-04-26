/*
 * $Id: semaphore_sem_timedwait.c,v 1.0 2021-01-18 17:08:03 clib2devs Exp $
 */

#include "semaphore_private.h"

static struct TimeRequest *
__open_timer(void) {
    struct MsgPort *mp;
    struct TimeRequest *tr;

    mp = (struct MsgPort *) AllocSysObjectTags(ASOT_PORT, ASOPORT_Signal, SIGBREAKB_CTRL_C, TAG_END);
    if (mp != NULL) {
        tr = (struct TimeRequest *) AllocSysObjectTags(ASOT_IOREQUEST, ASOIOR_ReplyPort, mp, ASOIOR_Size, sizeof(*tr),
                                                       TAG_END);
        if (tr != NULL) {
            if (OpenDevice("timer.device", UNIT_MICROHZ, (struct IORequest *) tr, 0) == IOERR_SUCCESS)
                return tr;

            FreeSysObject(ASOT_IOREQUEST, tr);
        }

        FreeSysObject(ASOT_PORT, mp);
    }

    return NULL;
}

static void
__close_timer(struct TimeRequest *tr) {
    struct MsgPort *mp = tr->Request.io_Message.mn_ReplyPort;

    CloseDevice((struct IORequest *) tr);

    FreeSysObject(ASOT_IOREQUEST, tr);
    FreeSysObject(ASOT_PORT, mp);
}

int
sem_timedwait(sem_t *sem, const struct timespec *abs_timeout) {
    isem_t *isem = (isem_t *) *sem;
    int ret = -1;

    ENTER();

    SHOWPOINTER(sem);
    SHOWPOINTER(abs_timeout);

    if (isem == NULL) {
        __set_errno(EINVAL);
        goto out;
    }

    ObtainSemaphore(&isem->accesslock);
    if (isem->value == 0) {
        struct Task *thistask = FindTask(NULL);
        struct TimeRequest *tr;
        struct timeval tv;
        int64 timediff;
        struct Node waitnode;
        uint32 signals;

        tr = __open_timer();
        if (tr == NULL) {
            ReleaseSemaphore(&isem->accesslock);
            __set_errno(ENOMEM);
            goto out;
        }

        gettimeofday(&tv, NULL);
        timediff = ((int64) abs_timeout->tv_sec * 1000000000LL + abs_timeout->tv_nsec) -
                   ((int64) tv.tv_sec * 1000000000LL + tv.tv_usec * 1000);
        if (timediff <= 0) {
            ReleaseSemaphore(&isem->accesslock);
            __close_timer(tr);
            __set_errno(ETIMEDOUT);
            goto out;
        }

        waitnode.ln_Pri = thistask->tc_Node.ln_Pri;
        waitnode.ln_Name = (STRPTR) thistask;

        SetSignal(0, SIGF_SINGLE);
        Enqueue(&isem->waitlist, &waitnode);

        tr->Request.io_Command = TR_ADDREQUEST;
        tr->Time.Seconds = timediff / 1000000000LL;
        tr->Time.Microseconds = (timediff % 1000000000LL) / 1000;

        SendIO((struct IORequest *) tr);

        do {
            ReleaseSemaphore(&isem->accesslock);
            signals = Wait(SIGBREAKF_CTRL_C | SIGF_SINGLE);
            ObtainSemaphore(&isem->accesslock);
        } while (isem->value == 0 && (signals & SIGBREAKF_CTRL_C) == 0);

        if (isem->value == 0) {
            Remove(&waitnode);
            ReleaseSemaphore(&isem->accesslock);

            if (CheckIO((struct IORequest *) tr)) {
                __set_errno(ETIMEDOUT);
            } else {
                AbortIO((struct IORequest *) tr);
                __set_errno(EINTR);
            }
            WaitIO((struct IORequest *) tr);

            __close_timer(tr);
            goto out;
        }

        __close_timer(tr);
    }
    isem->value--;
    ReleaseSemaphore(&isem->accesslock);

    ret = 0;
out:
    RETURN(ret);
    return(ret);
}
