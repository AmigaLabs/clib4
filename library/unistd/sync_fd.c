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

    /* The mode tells us what to flush. 0 means "flush just the data", and
       everything else means "flush everything. */
    Flush(fd->fd_File);

    if (mode != 0) {
        struct FileHandle *fh = BADDR(fd->fd_File);

        /* Verify that this file is not bound to "NIL:". */
        if (fh->fh_MsgPort != NULL)
            DoPkt(fh->fh_MsgPort, ACTION_FLUSH, 0, 0, 0, 0, 0);
    }

    result = OK;

out:

    __fd_unlock(fd);

    return (result);
}
