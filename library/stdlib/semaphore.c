/*
 * $Id: stdlib_semaphore.c,v 1.3 2021-01-31 12:04:26 clib2devs Exp $
*/

#include "stdlib_headers.h"

struct SignalSemaphore *
__create_semaphore(void) {
    struct SignalSemaphore *semaphore;

    ENTER();

#if 1
    semaphore = AllocVecTags(sizeof(*semaphore), AVT_Type, MEMF_SHARED, TAG_DONE);
    if (semaphore != NULL)
        InitSemaphore(semaphore);
#else
    semaphore = AllocSysObjectTags(ASOT_SEMAPHORE,
                                   ASOSEM_Size, sizeof(*semaphore),
                                   TAG_END);
#endif

    SHOWPOINTER(semaphore);
    LEAVE();

    return (semaphore);
}

/****************************************************************************/

void
__delete_semaphore(struct SignalSemaphore *semaphore) {
    ENTER();
    SHOWPOINTER(semaphore);

    if (semaphore != NULL) {
#if 1
        FreeVec(semaphore);
#else
        FreeSysObject(ASOT_SEMAPHORE, semaphore);
#endif
        semaphore = NULL;
    }

    LEAVE();
}
