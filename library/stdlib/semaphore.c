/*
 * $Id: stdlib_semaphore.c,v 1.3 2021-01-31 12:04:26 clib2devs Exp $
*/

#include "stdlib_headers.h"

struct SignalSemaphore *
__create_semaphore(void) {
    struct SignalSemaphore *semaphore;

    ENTER();

    semaphore = AllocSysObjectTags(ASOT_SEMAPHORE, ASOSEM_Pri, 0, TAG_END);

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
        FreeSysObject(ASOT_SEMAPHORE, semaphore);
        semaphore = NULL;
    }

    LEAVE();
}
