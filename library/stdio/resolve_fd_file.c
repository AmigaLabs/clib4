/*
 * $Id: stdio_resolve_fd_file.c,v 1.1 2006-11-16 14:39:23 clib2devs Exp $
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
        return ZERO;

    if (IDOS == NULL)
        return ZERO;

    /* Is this one the standard I/O streams for which the associated file
       handle should be determined dynamically? */
    if (FLAG_IS_SET(fd->fd_Flags, FDF_STDIO)) {
        switch (fd->fd_File) {
            case STDIN_FILENO:

                file = Input();
                break;

            case STDOUT_FILENO:

                file = Output();
                break;

            case STDERR_FILENO:

                file = ErrorOutput();

                /* The following is rather controversial; if the standard error stream
                       is unavailable, we default to reuse the standard output stream. This
                       is problematic if the standard output stream was redirected and should
                       not be the same as the standard error output stream. */
                if (file == ZERO)
                    file = Output();

                break;

            default:

                file = ZERO;
                break;
        }
    } else {
        /* Just return what's there. */
        file = fd->fd_File;
    }

    return (file);
}
