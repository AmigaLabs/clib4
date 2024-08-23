/*
 * $Id: fcntl_lock.c,v 1.4 2006-01-08 12:04:22 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
__fd_lock(struct fd *fd) {
    if (NULL != fd && NULL != fd->fd_Lock)
        ObtainSemaphore(fd->fd_Lock);
}

void
__fd_attemptLock(struct fd *fd) {
    if (NULL != fd && NULL != fd->fd_Lock)
        AttemptSemaphore(fd->fd_Lock);
}

void
__fd_unlock(struct fd *fd) {
    if (NULL != fd && NULL != fd->fd_Lock)
        ReleaseSemaphore(fd->fd_Lock);
}
