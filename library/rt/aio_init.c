/*
 * $Id: aio_aio_init.c,v 1.0 2023-03-03 07:15:37 clib4devs Exp $
*/
#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#include <aio.h>
#include "aio_misc.h"

/* Used by aio functions */
struct SignalSemaphore *__aio_lock;
CList *aio_threads;

#ifdef __USE_GNU
void aio_init(const struct aioinit *init) {
    return __aio_init(init);
}
#endif

CLIB_CONSTRUCTOR(aio_init) {
    ENTER();

    /* Initialize aio pthread list */
    __aio_lock = malloc(sizeof(struct SignalSemaphore));
    if (__aio_lock != NULL)
        InitSemaphore(__aio_lock);
    else
        goto out;

    /* Initialize aio list */
    aio_threads = CList_init(sizeof(struct AioThread));

out:

    LEAVE();
}

CLIB_DESTRUCTOR(aio_exit) {
    ENTER();

    /* Check if we have some aio threads */
    SHOWMSG("Check if we have some aio pthreads created");
    AioThread *aioThread;
    SHOWMSG("Obtain aio semaphore");
    ObtainSemaphore(__aio_lock);
    int streams = aio_threads->count(aio_threads);
    D(("AIO list has %ld items", streams));
    if (streams > 0) {
        for (int i = 0; i < streams; i++) {
            aioThread = aio_threads->at(aio_threads, i);
            D(("Cancel AIO stream with filedes %ld", aioThread->fileDes));
            aio_cancel(aioThread->fileDes, aioThread->aiocbp);
            Signal((struct Task *) aioThread->thread, SIGBREAKF_CTRL_C);
        }
    }
    ReleaseSemaphore(__aio_lock);

    /* Free aio list */
    if (aio_threads != NULL) {
        SHOWMSG("Free aio list");
        ObtainSemaphore(__aio_lock);
        aio_threads->free(aio_threads);
        ReleaseSemaphore(__aio_lock);
    }

    /* Remove aio semaphore. */
    free(__aio_lock);

    LEAVE();
}