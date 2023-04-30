/*
 * $Id: unistd_isatty.c,v 1.11 2010-10-20 13:12:59 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
isatty(int file_descriptor) {
    int result = ERROR;
    struct fd *fd;

    ENTER();

    SHOWVALUE(file_descriptor);

    assert(file_descriptor >= 0 && file_descriptor < __CLIB2->__num_fd);
    assert(__CLIB2->__fd[file_descriptor] != NULL);
    assert(FLAG_IS_SET(__CLIB2->__fd[file_descriptor]->fd_Flags, FDF_IN_USE));

    __check_abort();

    __stdio_lock();

    fd = __get_file_descriptor(file_descriptor);
    if (fd == NULL) {
        __set_errno(EBADF);
        goto out;
    }

    __fd_lock(fd);

    if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET)) {
        __set_errno(ENOTTY);
        goto out;
    }

    result = 1;

    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_IS_INTERACTIVE)) {
        BPTR file;

        file = __resolve_fd_file(fd);
        if (file == ZERO || NOT IsInteractive(file))
        result = 0;
    }

out:

    __fd_unlock(fd);
    __stdio_unlock();

    RETURN(result);
    return (result);
}
