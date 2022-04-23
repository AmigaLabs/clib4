/*
 * $Id: semaphore_sem_getvalue.c,v 1.0 2021-01-18 17:08:03 clib2devs Exp $
 */

#include "semaphore_private.h"

int sem_getvalue(sem_t *sem, int *sval) {
    isem_t *isem = (isem_t *) *sem;

    ENTER();

    SHOWPOINTER(sem);
    SHOWPOINTER(sval);

    if (isem == NULL) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    *sval = (int) isem->value;
    RETURN(0);
    return 0;
}
