/*
 * $Id: unistd_isatty.c,v 1.12 2024-03-30 13:12:59 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
isatty(int file_descriptor) {
    int result = ERROR;
    struct fd *fd;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(file_descriptor);

    assert(file_descriptor >= 0 && file_descriptor < __clib4->__num_fd);
    assert(__clib4->__fd[file_descriptor] != NULL);
    assert(FLAG_IS_SET(__clib4->__fd[file_descriptor]->fd_Flags, FDF_IN_USE));

    __check_abort_f(__clib4);

    __stdio_lock(__clib4);

    fd = __get_file_descriptor(__clib4, file_descriptor);
    if (fd == NULL) {
        __set_errno(EBADF);
        goto out;
    }

    __fd_lock(fd);

    if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET)) {
        __fd_unlock(fd);
        __set_errno(ENOTTY);
        goto out;
    }

    result = 1;

    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_IS_INTERACTIVE) && FLAG_IS_CLEAR(fd->fd_Flags, FDF_IS_SERIAL)) {
        BPTR file;

        file = __resolve_fd_file(fd);
        if (file == BZERO || NOT IsInteractive(file))
            result = 0;
    }

    __fd_unlock(fd);

out:

    __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}
