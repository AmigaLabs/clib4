/*
 * $Id: aio_aio_suspend.c,v 1.0 2023-03-03 07:15:37 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#include <aio.h>
#include <pthread.h>
#include <sys/param.h>

#include "aio_misc.h"

struct clparam {
    const struct aiocb *const *list;
    struct waitlist *waitlist;
    struct requestlist **requestlist;
    pthread_cond_t *cond;
    int nent;
};


static void
cleanup(void *arg) {
    const struct clparam *param = (const struct clparam *) arg;

    /* Now remove the entry in the waiting list for all requests which didn't terminate.  */
    int cnt = param->nent;
    while (cnt-- > 0) {
        if (param->list[cnt] != NULL && param->list[cnt]->__error_code == EINPROGRESS) {
            struct waitlist **listp;

            assert(param->requestlist[cnt] != NULL);

            /* There is the chance that we cannot find our entry anymore. This
               could happen if the request terminated and restarted again.  */
            listp = &param->requestlist[cnt]->waiting;
            while (*listp != NULL && *listp != &param->waitlist[cnt])
                listp = &(*listp)->next;

            if (*listp != NULL)
                *listp = (*listp)->next;
        }
    }

    /* Release the conditional variable.  */
    pthread_cond_destroy(param->cond);

    /* Release the mutex.  */
    pthread_mutex_unlock(&__aio_requests_mutex);
}

int
__aio_suspend_time64(const struct aiocb *const list[], int nent, const struct timespec64 *timeout) {
    ENTER();
    SHOWVALUE(nent);

    if (nent < 0) {
        errno = EINVAL;
        RETURN(-1);
        return -1;
    }

    struct waitlist waitlist[nent];
    struct requestlist *requestlist[nent];
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    int cnt;
    bool any = false;
    int result = 0;
    unsigned int cntr = 1;

    /* Request the mutex.  */
    pthread_mutex_lock(&__aio_requests_mutex);

    /* There is not yet a finished request.  Signal the request that we are working for it.  */
    for (cnt = 0; cnt < nent; ++cnt) {
        if (list[cnt] != NULL) {
            SHOWMSG("Find an element to suspend");
            SHOWVALUE(list[cnt]->__error_code);
            if (list[cnt]->__error_code == EINPROGRESS) {
                SHOWMSG("In it EINPROGRESS");
                requestlist[cnt] = __aio_find_req((aiocb_union *) list[cnt]);

                if (requestlist[cnt] != NULL) {
                    waitlist[cnt].cond = &cond;
                    waitlist[cnt].result = NULL;
                    waitlist[cnt].next = requestlist[cnt]->waiting;
                    waitlist[cnt].counterp = &cntr;
                    waitlist[cnt].sigevp = NULL;
                    requestlist[cnt]->waiting = &waitlist[cnt];
                    any = true;
                } else
                    /* We will never suspend.  */
                    break;
            } else
                /* We will never suspend.  */
                break;
        }
    }

    struct timespec64 ts;
    if (timeout != NULL) {
        clock_gettime64(CLOCK_MONOTONIC, &ts);
        ts.tv_sec += timeout->tv_sec;
        ts.tv_nsec += timeout->tv_nsec;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_nsec -= 1000000000;
            ts.tv_sec++;
        }
    }
    SHOWVALUE(cnt);
    SHOWVALUE(nent);
    SHOWVALUE(any);
    /* Only if none of the entries is NULL or finished to be wait.  */
    if (cnt == nent && any) {
        struct clparam clparam = {
            .list = list,
            .waitlist = waitlist,
            .requestlist = requestlist,
            .cond = &cond,
            .nent = nent
        };

        SHOWMSG("Call pthread_cleanup_push");
        pthread_cleanup_push(cleanup, &clparam);

        struct timespec ts32 = valid_timespec64_to_timespec(ts);
        result = pthread_cond_timedwait(&cond, &__aio_requests_mutex, timeout == NULL ? NULL : &ts32);

        pthread_cleanup_pop(0);
    }

    /* Now remove the entry in the waiting list for all requests
       which didn't terminate.  */
    while (cnt-- > 0)
        if (list[cnt] != NULL && list[cnt]->__error_code == EINPROGRESS) {
            struct waitlist **listp;

            assert(requestlist[cnt] != NULL);

            /* There is the chance that we cannot find our entry anymore. This
               could happen if the request terminated and restarted again.  */
            listp = &requestlist[cnt]->waiting;
            while (*listp != NULL && *listp != &waitlist[cnt])
                listp = &(*listp)->next;

            if (*listp != NULL)
                *listp = (*listp)->next;
        }

    /* Release the conditional variable.  */
    if (pthread_cond_destroy(&cond) != 0)
        /* This must never happen.  */
        abort();

    if (result != 0) {
        /* An error occurred.  Possibly it's ETIMEDOUT.  We have to translate
       the timeout error report of `pthread_cond_timedwait' to the
       form expected from `aio_suspend'.  */
        if (result == ETIMEDOUT)
            errno = EAGAIN;
        else
            errno = result;

        result = -1;
    }

    /* Release the mutex.  */
    pthread_mutex_unlock(&__aio_requests_mutex);

    RETURN(result);
    return result;
}

int
aio_suspend(const struct aiocb *const list[], int nent, const struct timespec *timeout) {
    struct timespec64 ts64;

    if (timeout != NULL)
        ts64 = valid_timespec_to_timespec64(*timeout);

    return __aio_suspend_time64(list, nent, timeout != NULL ? &ts64 : NULL);
}
