/*
 * $Id: aio_lio_listio.c,v 1.0 2023-03-03 07:15:37 clib2devs Exp $
*/
#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <aio.h>
#include <pthread.h>
#include "aio_misc.h"

#define LIO_OPCODE_BASE 0
#define LIO_MODE(mode) mode
#define NO_INDIVIDUAL_EVENT_P(mode) 0

/* We need this special structure to handle asynchronous I/O.  */
struct async_waitlist {
    unsigned int counter;
    struct sigevent sigev;
    struct waitlist list[0];
};

static int
lio_listio_internal(int mode, struct aiocb *const list[], int nent, struct sigevent *sig) {
    struct sigevent defsigev;
    struct requestlist *requests[nent];
    int cnt;
    volatile unsigned int total = 0;
    int result = 0;

    if (sig == NULL) {
        defsigev.sigev_notify = SIGEV_NONE;
        sig = &defsigev;
    }

    /* Request the mutex.  */
    pthread_mutex_lock(&__aio_requests_mutex);

    /* Now we can enqueue all requests.  Since we already acquired the
       mutex the enqueue function need not do this.  */
    for (cnt = 0; cnt < nent; ++cnt)
        if (list[cnt] != NULL && list[cnt]->aio_lio_opcode != LIO_NOP) {
            if (NO_INDIVIDUAL_EVENT_P(mode))
                list[cnt]->aio_sigevent.sigev_notify = SIGEV_NONE;

            requests[cnt] = __aio_enqueue_request((aiocb_union *) list[cnt],
                                                  (list[cnt]->aio_lio_opcode
                                                   | LIO_OPCODE_BASE));

            if (requests[cnt] != NULL)
                /* Successfully enqueued.  */
                ++total;
            else
                /* Signal that we've seen an error.  `errno' and the error code
                   of the aiocb will tell more.  */
                result = -1;
        } else
            requests[cnt] = NULL;

    if (total == 0) {
        /* We don't have anything to do except signalling if we work
       asynchronously.  */

        /* Release the mutex.  We do this before raising a signal since the
       signal handler might do a `siglongjmp' and then the mutex is
       locked forever.  */
        pthread_mutex_unlock(&__aio_requests_mutex);

        if (LIO_MODE(mode) == LIO_NOWAIT)
            __aio_notify_only(sig);

        return result;
    } else if (LIO_MODE(mode) == LIO_WAIT) {
#ifndef DONT_NEED_AIO_MISC_COND
        pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
        int oldstate;
#endif
        struct waitlist waitlist[nent];

        total = 0;
        for (cnt = 0; cnt < nent; ++cnt) {
            assert(requests[cnt] == NULL || list[cnt] != NULL);

            if (requests[cnt] != NULL && list[cnt]->aio_lio_opcode != LIO_NOP) {
#ifndef DONT_NEED_AIO_MISC_COND
                waitlist[cnt].cond = &cond;
#endif
                waitlist[cnt].result = &result;
                waitlist[cnt].next = requests[cnt]->waiting;
                waitlist[cnt].counterp = &total;
                waitlist[cnt].sigevp = NULL;
                requests[cnt]->waiting = &waitlist[cnt];
                ++total;
            }
        }

#ifdef DONT_NEED_AIO_MISC_COND
        AIO_MISC_WAIT (result, total, NULL, 0);
#else
        /* Since `pthread_cond_wait'/`pthread_cond_timedwait' are cancellation
       points we must be careful.  We added entries to the waiting lists
       which we must remove.  So defer cancellation for now.  */
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);

        while (total > 0)
            pthread_cond_wait(&cond, &__aio_requests_mutex);

        /* Now it's time to restore the cancellation state.  */
        pthread_setcancelstate(oldstate, NULL);

        /* Release the conditional variable.  */
        if (pthread_cond_destroy(&cond) != 0)
            /* This must never happen.  */
            abort();
#endif

        /* If any of the I/O requests failed, return -1 and set errno.  */
        if (result != 0) {
            __set_errno(result == EINTR ? EINTR : EIO);
            result = -1;
        }
    } else {
        struct async_waitlist *waitlist;

        waitlist = (struct async_waitlist *)
                malloc(sizeof(struct async_waitlist)
                       + (nent * sizeof(struct waitlist)));

        if (waitlist == NULL) {
            __set_errno(EAGAIN);
            result = -1;
        } else {
            total = 0;

            for (cnt = 0; cnt < nent; ++cnt) {
                assert(requests[cnt] == NULL || list[cnt] != NULL);

                if (requests[cnt] != NULL
                    && list[cnt]->aio_lio_opcode != LIO_NOP) {
#ifndef DONT_NEED_AIO_MISC_COND
                    waitlist->list[cnt].cond = NULL;
#endif
                    waitlist->list[cnt].result = NULL;
                    waitlist->list[cnt].next = requests[cnt]->waiting;
                    waitlist->list[cnt].counterp = &waitlist->counter;
                    waitlist->list[cnt].sigevp = &waitlist->sigev;
                    requests[cnt]->waiting = &waitlist->list[cnt];
                    ++total;
                }
            }

            waitlist->counter = total;
            waitlist->sigev = *sig;
        }
    }

    /* Release the mutex.  */
    pthread_mutex_unlock(&__aio_requests_mutex);

    return result;
}

int
lio_listio(int mode, struct aiocb *const list[], int nent, struct sigevent *sig) {
    /* Check arguments.  */
    if (mode != LIO_WAIT && mode != LIO_NOWAIT) {
        __set_errno(EINVAL);
        return -1;
    }

    return lio_listio_internal(mode, list, nent, sig);
}