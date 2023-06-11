/*
 * $Id: termios_tcdrain.c,v 1.4 2006-11-16 14:39:23 clib2devs Exp $
*/

#ifndef    _TERMIOS_HEADERS_H
#include "termios_headers.h"
#endif /* _TERMIOS_HEADERS_H */

#if !defined(Flush)
#define Flush(fh) FFlush(fh)
#endif /* !Flush */

int
tcdrain(int file_descriptor) {
    int result = ERROR;
    struct fd *fd;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    SHOWVALUE(file_descriptor);

    __check_abort();

    __stdio_lock(__clib2);

    fd = __get_file_descriptor(file_descriptor);
    if (fd == NULL) {
        __set_errno(EBADF);
        goto out;
    }

    __fd_lock(fd);

    if (FLAG_IS_SET(fd->fd_Flags, FDF_TERMIOS)) {
        struct termios *tios;
        BPTR file;

        tios = fd->fd_Aux;

        switch (tios->type) {
            case TIOST_CONSOLE:
                file = __resolve_fd_file(fd);
                if (file == BZERO) {
                    __fd_unlock(fd);
                    __set_errno(EBADF);
                    goto out;
                }

                /* This also discards any buffered input, but it does
                   not appear possible to drain the output buffer
                   otherwise. (?) */
                if (CANNOT Flush(file))
                    goto out;

                break;
            default: /* TODO: Serial port support. */
                printf("cicero1b\n");
                __fd_unlock(fd);
                __set_errno(ENXIO);
                goto out;
        }

        result = OK;
    } else {
        result = fdatasync(file_descriptor); /* If called on a "regular" file. */
    }

    __fd_unlock(fd);

out:

    __stdio_unlock(__clib2);

    RETURN(result);
    return (result);
}
