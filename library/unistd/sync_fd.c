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
    (void)mode;

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

    /* Flush the dos.library file buffer to the filesystem handler.
       FFlush sends buffered data to the handler, which is sufficient
       for both data-only (mode==0) and full sync (mode!=0) on AmigaOS.
       We do NOT send a raw ACTION_FLUSH packet via DoPkt because some
       filesystems (e.g. SmartFilesystem) crash when receiving it. */
    Flush(fd->fd_File);

    result = OK;

out:

    __fd_unlock(fd);

    return (result);
}
