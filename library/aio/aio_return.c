/*
 * $Id: aio_aio_return.c,v 1.0 2023-03-03 07:15:37 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <aio.h>
#include <pthread.h>
#include "aio_misc.h"

ssize_t
aio_return(struct aiocb *aiocbp) {
    return aiocbp->__return_value;
}