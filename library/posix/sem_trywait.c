/*
 * $Id: semaphore_sem_trywait.c,v 1.0 2021-01-18 17:08:03 clib2devs Exp $
 */

#include "semaphore_private.h"

int sem_trywait(sem_t *sem) {
    int ret = -1;
    isem_t *isem = (isem_t *) *sem;

    ENTER();

    SHOWPOINTER(sem);

    if (isem == NULL) {
        __set_errno(EINVAL);
        goto out;
    }

    if (AttemptSemaphore(&isem->accesslock) == FALSE) {
        __set_errno(EAGAIN);
        goto out;
    }
    if (isem->value == 0) {
        ReleaseSemaphore(&isem->accesslock);
        __set_errno(EAGAIN);
        goto out;
    }
    isem->value--;
    ReleaseSemaphore(&isem->accesslock);

    ret = 0;

out:
    RETURN(ret);
    return(ret);
}

