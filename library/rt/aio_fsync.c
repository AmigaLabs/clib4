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
aio_fsync(int op, struct aiocb *aiocbp) {
    if (op != O_DSYNC && __builtin_expect(op != O_SYNC, 0)) {
        __set_errno(EINVAL);
        return -1;
    }

    /* Verify that this is an open file descriptor.  */
    if (fcntl(aiocbp->aio_fildes, F_GETFL) == -1) {
        __set_errno(EBADF);
        return -1;
    }

    return (__aio_enqueue_request((aiocb_union *) aiocbp, op == O_SYNC ? LIO_SYNC : LIO_DSYNC) == NULL ? -1 : 0);
}