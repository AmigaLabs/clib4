/*
 * $Id: semaphore_sem_wait.c,v 1.0 2021-01-18 17:08:03 clib2devs Exp $
 */

#include "semaphore_private.h"

int
sem_wait(sem_t *sem) {
    isem_t *isem = (isem_t *) *sem;

    ENTER();

    SHOWPOINTER(sem);

    if (isem == NULL) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    ObtainSemaphore(&isem->accesslock);
    if (isem->value == 0) {
        struct Task *thistask = FindTask(NULL);
        struct Node waitnode;
        uint32 signals;

        waitnode.ln_Pri = thistask->tc_Node.ln_Pri;
        waitnode.ln_Name = (STRPTR) thistask;

        Enqueue(&isem->waitlist, &waitnode);

        do {
            SetSignal(0, SIGF_SINGLE);
            ReleaseSemaphore(&isem->accesslock);
            signals = Wait(SIGBREAKF_CTRL_C | SIGF_SINGLE);
            ObtainSemaphore(&isem->accesslock);
        } while (isem->value == 0 && (signals & SIGBREAKF_CTRL_C) == 0);

        if (isem->value == 0) {
            Remove(&waitnode);
            ReleaseSemaphore(&isem->accesslock);
            __set_errno(EINTR);
            RETURN(-1);
            return -1;
        }
    }
    isem->value--;
    ReleaseSemaphore(&isem->accesslock);

    RETURN(0);
    return 0;
}
