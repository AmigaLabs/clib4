/*
 * $Id: fcntl_get_default_file.c,v 1.8 2006-11-16 14:39:23 clib4devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#include "../fcntl/fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

int
__get_default_file(int file_descriptor, long *file_ptr) {
    int result = ERROR;
    struct fd *fd;
    struct _clib4 *__clib4 = __CLIB4;

    assert(file_descriptor >= 0 && file_descriptor < __clib4->__num_fd);
    assert(__clib4->__fd[file_descriptor] != NULL);
    assert(FLAG_IS_SET(__clib4->__fd[file_descriptor]->fd_Flags, FDF_IN_USE));
    assert(file_ptr != NULL);

    fd = __get_file_descriptor(__clib4, file_descriptor);
    if (fd == NULL) {
        __set_errno(EBADF);
        goto out;
    }

    __fd_lock(fd);

    (*file_ptr) = (long) __resolve_fd_file(fd);

    result = OK;

out:

    __fd_unlock(fd);

    return (result);
}
