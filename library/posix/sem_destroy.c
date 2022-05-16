/*
 * $Id: semaphore_sem_destroy.c,v 1.0 2021-01-18 17:08:03 clib2devs Exp $
 */

#include "semaphore_private.h"

int sem_destroy(sem_t *sem) {
    isem_t *isem = *(isem_t **) sem;

    ENTER();

    SHOWPOINTER(sem);

    if (isem->accesslock != NULL) {
        FreeVec(isem->accesslock);
        isem->accesslock = NULL;
    }

    if (isem != NULL) {
        FreeVec(isem);
        *sem = NULL;
    }

    RETURN(0);
    return 0;
}
