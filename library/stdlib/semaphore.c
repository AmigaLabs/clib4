/*
 * $Id: stdlib_semaphore.c,v 1.4 2025-06-28 12:04:26 clib4devs Exp $
*/

#include "stdlib_headers.h"

struct SignalSemaphore *
__create_semaphore(void) {
    struct SignalSemaphore *semaphore;

    ENTER();

    semaphore = AllocSysObjectTags(ASOT_SEMAPHORE, ASOSEM_Pri, 0, TAG_END);

    RETURN(semaphore);
    return semaphore;
}

void
__delete_semaphore(struct SignalSemaphore *semaphore) {
    ENTER();
    SHOWPOINTER(semaphore);

    FreeSysObject(ASOT_SEMAPHORE, semaphore);

    LEAVE();
}

APTR
__create_mutex(void) {
    ENTER();

    APTR mutex = AllocSysObjectTags(ASOT_MUTEX, TAG_END);

    RETURN(mutex);
    return mutex;
}

void
__delete_mutex(APTR mutex) {
    ENTER();
    SHOWPOINTER(mutex);

    FreeSysObject(ASOT_MUTEX, mutex);

    LEAVE();
}
