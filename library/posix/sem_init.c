/*
 * $Id: semaphore_sem_init.c,v 1.0 2021-01-18 17:08:03 clib2devs Exp $
 */

#include "semaphore_private.h"

int
sem_init(sem_t *sem, int pshared, unsigned int value) {
    isem_t *isem;
    (void) (pshared);

    ENTER();

    SHOWPOINTER(sem);
    SHOWVALUE(pshared);
    SHOWVALUE(value);

    isem = malloc(sizeof(isem_t));
    if (isem == NULL) {
        __set_errno(ENOMEM);
        RETURN(-1);
        return -1;
    }

    isem->accesslock = AllocVecTags(sizeof(isem->accesslock), AVT_Type, MEMF_SHARED, TAG_DONE);
    if (isem->accesslock == NULL) {
        __set_errno(ENOMEM);
        RETURN(-1);
        return -1;
    }

    InitSemaphore(isem->accesslock);
    NewList(&isem->waitlist);
    isem->value = value;

    *sem = isem;

    RETURN(0);
    return 0;
}
