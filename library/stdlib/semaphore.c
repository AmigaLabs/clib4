/*
 * $Id: stdlib_semaphore.c,v 1.3 2021-01-31 12:04:26 clib2devs Exp $
*/

#include "stdlib_headers.h"

struct SignalSemaphore *
__create_semaphore(void) {
    struct SignalSemaphore *semaphore;

    semaphore = AllocVecTags(sizeof(*semaphore), AVT_Type, MEMF_SHARED, TAG_DONE);
    if (semaphore != NULL)
        InitSemaphore(semaphore);

    return (semaphore);
}

/****************************************************************************/

void
__delete_semaphore(struct SignalSemaphore *semaphore) {
    if (semaphore != NULL) {
        FreeVec(semaphore);
        //FreeSysObject(ASOT_SEMAPHORE,semaphore);
    }
}
