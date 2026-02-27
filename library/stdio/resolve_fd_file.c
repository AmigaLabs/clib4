/*
 * $Id: stdio_resolve_fd_file.c,v 1.2 2025-04-01 14:39:23 clib4devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#include "../fcntl/fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

#ifdef __resolve_fd_file
#undef __resolve_fd_file
#endif /* __resolve_fd_file */

BPTR
__resolve_fd_file(struct fd *fd) {
    BPTR file;

    if (fd == NULL)
        return BZERO;

    file = fd->fd_File;

    return file;
}
