/*
 * $Id: termios_tcdrain.c,v 1.4 2006-11-16 14:39:23 clib4devs Exp $
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
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(file_descriptor);

    __check_abort_f(__clib4);

    __stdio_lock(__clib4);

    fd = __get_file_descriptor(__clib4, file_descriptor);
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
                if (CANNOT Flush(file)) {
                    __fd_unlock(fd);
                    goto out;
                }

                break;
            default: /* TODO: Serial port support. */
                __fd_unlock(fd);
                __set_errno(ENXIO);
                goto out;
        }

        result = OK;

        __fd_unlock(fd);
    } else {
        __stdio_unlock(__clib4);
        __fd_unlock(fd);
        SHOWMSG("Calling fdatasync");
        LEAVE();
        return fdatasync(file_descriptor); /* If called on a "regular" file. */
    }

out:

    __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}
