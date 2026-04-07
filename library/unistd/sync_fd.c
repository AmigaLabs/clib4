/*
 * $Id: unistd_sync_fd.c,v 1.8 2006-11-16 14:39:23 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#if !defined(Flush)
#define Flush(fh) FFlush(fh)
#endif /* !Flush */

int
__sync_fd(struct fd *fd, int mode) {
    int result = ERROR;

    assert(fd != NULL);

    __fd_lock(fd);

    if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET)) {
        __set_errno(EINVAL);
        goto out;
    }

    if (FLAG_IS_SET(fd->fd_Flags, FDF_STDIO)) {
        __set_errno(EBADF);
        goto out;
    }

    if (fd->fd_File == ZERO) {
        __set_errno(EBADF);
        goto out;
    }

    /* Flush the dos.library file buffer to the filesystem handler. */
    Flush(fd->fd_File);

    if (mode != 0) {
        /* Full sync requested (fsync): also ask the filesystem to flush
           its internal caches to disk via the proper DOS API.
           We use FlushVolumePort() instead of raw DoPkt(ACTION_FLUSH)
           because some filesystems (e.g. SmartFilesystem) crash with
           the raw packet. */
        struct FileHandle *fh = BADDR(fd->fd_File);
        if (fh != NULL && fh->fh_MsgPort != NULL)
            FlushVolumePort(fh->fh_MsgPort);
    }

    result = OK;

out:

    __fd_unlock(fd);

    return (result);
}
