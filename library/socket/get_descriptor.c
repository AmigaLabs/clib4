/*
 * $Id: socket_get_descriptor.c,v 1.6 2024-07-04 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct fd *
__get_file_descriptor_socket(struct _clib4 *__clib4, int socket_descriptor) {
    struct fd *result = NULL;
    struct fd *fd;

    fd = __get_file_descriptor(__clib4, socket_descriptor);
    if (fd == NULL) {
        __set_errno(EBADF);
        goto out;
    }

    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_IS_SOCKET)) {
        __set_errno(ENOTSOCK);
        goto out;
    }

    result = fd;

out:

    return (result);
}
