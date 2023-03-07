/*
 * $Id: aio_aio_cancel.c,v 1.0 2023-03-03 07:15:37 clib2devs Exp $
*/
#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <aio.h>
#include <pthread.h>
#include "aio_misc.h"

int
aio_cancel(int fildes, struct aiocb *aiocbp) {
    struct requestlist *req = NULL;
    int result = AIO_ALLDONE;

    /* If fildes is invalid, error. */
    if (fcntl(fildes, F_GETFL) < 0) {
        __set_errno(EBADF);
        return -1;
    }

    /* Request the mutex.  */
    pthread_mutex_lock(&__aio_requests_mutex);

    /* We are asked to cancel a specific AIO request.  */
    if (aiocbp != NULL) {
        /* If the AIO request is not for this descriptor it has no value
       to look for the request block.  */
        if (aiocbp->aio_fildes != fildes) {
            pthread_mutex_unlock(&__aio_requests_mutex);
            __set_errno(EINVAL);
            return -1;
        } else if (aiocbp->__error_code == EINPROGRESS) {
            struct requestlist *last = NULL;

            req = __aio_find_req_fd(fildes);

            if (req == NULL) {
                not_found:
                pthread_mutex_unlock(&__aio_requests_mutex);
                __set_errno(EINVAL);
                return -1;
            }

            while (req->aiocbp != (aiocb_union *) aiocbp) {
                last = req;
                req = req->next_prio;
                if (req == NULL)
                    goto not_found;
            }

            /* Don't remove the entry if a thread is already working on it.  */
            if (req->running == allocated) {
                result = AIO_NOTCANCELED;
                req = NULL;
            } else {
                /* We can remove the entry.  */
                __aio_remove_request(last, req, 0);

                result = AIO_CANCELED;

                req->next_prio = NULL;
            }
        }
    } else {
        /* Find the beginning of the list of all requests for this
       desriptor.  */
        req = __aio_find_req_fd(fildes);

        /* If any request is worked on by a thread it must be the first.
       So either we can delete all requests or all but the first.  */
        if (req != NULL) {
            if (req->running == allocated) {
                struct requestlist *old = req;
                req = req->next_prio;
                old->next_prio = NULL;

                result = AIO_NOTCANCELED;

                if (req != NULL)
                    __aio_remove_request(old, req, 1);
            } else {
                result = AIO_CANCELED;

                /* We can remove the entry.  */
                __aio_remove_request(NULL, req, 1);
            }
        }
    }

    /* Mark requests as canceled and send signal.  */
    while (req != NULL) {
        struct requestlist *old = req;
        assert(req->running == yes || req->running == queued);
        req->aiocbp->aiocb.__error_code = ECANCELED;
        req->aiocbp->aiocb.__return_value = -1;
        __aio_notify(req);
        req = req->next_prio;
        __aio_free_request(old);
    }

    /* Release the mutex.  */
    pthread_mutex_unlock(&__aio_requests_mutex);

    return result;
}
