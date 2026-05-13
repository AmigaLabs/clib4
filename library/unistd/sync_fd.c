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
    BPTR file;

    assert(fd != NULL);

    __fd_lock(fd);

    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE)) {
        __set_errno(EBADF);
        goto out;
    }

    if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET)) {
        __set_errno(EINVAL);
        goto out;
    }

    if (FLAG_IS_SET(fd->fd_Flags, FDF_STDIO)) {
        __set_errno(EBADF);
        goto out;
    }

    if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_DIRECTORY) ||
        FLAG_IS_SET(fd->fd_Flags, FDF_PATH_ONLY)) {
        __set_errno(EBADF);
        goto out;
    }

    file = __resolve_fd_file(fd);
    if (file == BZERO) {
        __set_errno(EBADF);
        goto out;
    }

    /* Flush the dos.library file buffer to the filesystem handler. */
    Flush(file);
#if 0
    if (mode != 0) {
        /* Full sync requested (fsync): also ask the filesystem to flush
           its internal caches to disk via the proper DOS API.
           We use DevNameFromFH + FlushVolume instead of accessing the
           FileHandle struct directly (BADDR + fh_MsgPort) because the
           internal FileHandle layout may not be safe to access. */
        TEXT devname[256] = {0};
        if (DevNameFromFH(file, devname, sizeof(devname), DN_DEVICEONLY))
            FlushVolume(devname);
    }
#endif
    result = OK;

out:

    __fd_unlock(fd);

    return (result);
}
