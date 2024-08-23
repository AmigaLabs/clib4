/*
 * $Id: termios_tcsendbreak.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef    _TERMIOS_HEADERS_H
#include "termios_headers.h"
#endif /* _TERMIOS_HEADERS_H */

/****************************************************************************/

/*
 * tcsendbreak() has a rather odd time specification.
 * If duration is 0, it will send a BREAK for 0.25-0.5 seconds
 * and if duration is non-zero...
 * 	Linux asserts BREAK for N*duration where N is somewhere between 0.25 and 0.5 seconds.
 * 	Solaris calls tcdrain() instead. (According to the manual)
 * Let us try the Solaris way and see how it goes.
 *
 * If the file descriptor does not referr to a serial port, no action is needed.
 */

int
tcsendbreak(int file_descriptor, int duration) {
    int result = ERROR;
    struct fd *fd;
    struct termios *tios;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWVALUE(duration);

    __check_abort_f(__clib4);

    __stdio_lock(__clib4);

    fd = __get_file_descriptor(__clib4, file_descriptor);
    if (fd == NULL || FLAG_IS_CLEAR(fd->fd_Flags, FDF_TERMIOS)) {
        __set_errno(EBADF);
        goto out;
    }

    __fd_lock(fd);

    assert(fd->fd_Aux != NULL);

    tios = fd->fd_Aux;

    switch (tios->type) {
        case TIOST_CONSOLE:
            result = OK;
            break;
        case TIOST_SERIAL:
            if (duration == 0) {
                /* TODO */
                result = OK;
            } else {
                __stdio_unlock(__clib4);
                __fd_unlock(fd);
                SHOWMSG("Calling tcdrain");
                LEAVE();
                return tcdrain(file_descriptor);
            }
            break;
        default:
            SHOWMSG("Invalid tios type in tcsendbreak.");
    }

    __fd_unlock(fd);

out:

    __stdio_unlock(__clib4);

    __check_abort_f(__clib4);

    RETURN(result);
    return (result);
}
