/*
 * $Id: semaphore_sem_post.c,v 1.0 2021-01-18 17:08:03 clib2devs Exp $
 */

#include "semaphore_private.h"

int
sem_post(sem_t *sem)
{
    isem_t *isem = (isem_t *)*sem;
    int ret = -1;

    ENTER();

    SHOWPOINTER(sem);

    if (isem == NULL)
    {
        __set_errno(EINVAL);
        return -1;
    }

    ObtainSemaphore(&isem->accesslock);
    if (isem->value == SEM_VALUE_MAX)
    {
        ReleaseSemaphore(&isem->accesslock);
        __set_errno(EOVERFLOW);
        return -1;
    }
    isem->value++;
    if (isem->value > 0)
    {
        struct Node *node;

        node = RemHead(&isem->waitlist);
        if (node)
        {
            Signal((struct Task *)node->ln_Name, SIGF_SINGLE);
        }
    }
    ReleaseSemaphore(&isem->accesslock);

    return 0;
}
