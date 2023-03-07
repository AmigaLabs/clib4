/*
 * $Id: aio_aio_error.c,v 1.0 2023-03-03 07:15:37 clib2devs Exp $
*/
#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <aio.h>
#include <pthread.h>
#include "aio_misc.h"

int
aio_error(const struct aiocb *aiocbp) {
    int ret;

    /* Acquire the mutex to make sure all operations for this request are complete.  */
    pthread_mutex_lock(&__aio_requests_mutex);
    ret = aiocbp->__error_code;
    pthread_mutex_unlock(&__aio_requests_mutex);

    return ret;
}