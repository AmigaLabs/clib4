/*
 * $Id: aio_aio_sigqueue.c,v 1.0 2023-03-03 07:15:37 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <aio.h>
#include <pthread.h>
#include "aio_misc.h"

int
__attribute__ ((visibility ("hidden")))
__aio_sigqueue(int sig, const union sigval val, pid_t caller_pid) {
    errno = ENOSYS;
    return -1;
}