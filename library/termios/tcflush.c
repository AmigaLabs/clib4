/*
 * $Id: termios_tcflush.c,v 1.5 2006-11-16 14:39:23 clib4devs Exp $
*/

#ifndef    _TERMIOS_HEADERS_H
#include "termios_headers.h"
#endif /* _TERMIOS_HEADERS_H */

/****************************************************************************/

/*
 * This is a bit messy and might possibly be done in a better way, but console
 * termios support is a bit of a hack at best.
 * tcflush() could possibly be implemented with using CMD_CLEAR with the
 * console.device, but this would be just as messy and possibly break
 * even more if e.g. stdin is a pipe.
 */

int
tcflush(int file_descriptor, int queue) {
    int result = ERROR;
    struct fd *fd;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(file_descriptor);

    __check_abort_f(__clib4);

    __stdio_lock(__clib4);

    fd = __get_file_descriptor(__clib4, file_descriptor);
    if (fd == NULL || FLAG_IS_CLEAR(fd->fd_Flags, FDF_TERMIOS)) {
        __set_errno(EBADF);
        goto out;
    }

    if (queue < TCIFLUSH || queue > TCIOFLUSH) {
        SHOWMSG("Invalid queue specified to tcflush().");

        __set_errno(EINVAL);
        goto out;
    }

    __fd_lock(fd);

    if (queue == TCIFLUSH || queue == TCIOFLUSH) {
        LONG num_bytes_read;
        char buf[64];
        struct termios *tios;
        BPTR file;

        tios = fd->fd_Aux;

        file = __resolve_fd_file(fd);
        if (file == BZERO) {
            __fd_unlock(fd);
            __set_errno(EBADF);
            goto out;
        }

        /* Set raw mode so we can read without blocking. */
        if (FLAG_IS_SET(tios->c_lflag, ICANON)) {
            SetMode(file, DOSTRUE);

            SET_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
        }

        while (WaitForChar(file, 1) != DOSFALSE) {
            if (__clib4->__check_abort_enabled && FLAG_IS_SET(SetSignal(0, 0), __clib4->__break_signal_mask))
                break;

            /* Read away available data. (upto 8k) */
            num_bytes_read = Read(file, buf, 64);
            if (num_bytes_read == ERROR || num_bytes_read == 0)
                break;
        }

        /* Restore the Raw/Cooked mode. */
        if (FLAG_IS_SET(tios->c_lflag, ICANON)) {
            SetMode(file, DOSFALSE); /* Set Cooked = Canonical mode. */
            CLEAR_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
        }

        result = OK;
    } else if (queue == TCOFLUSH || queue == TCIOFLUSH) {
        /* TODO: Can we actually discard data buffered on the file?
         * For now we do the same as tcdrain().
         */
        __stdio_unlock(__clib4);
        __fd_unlock(fd);
        SHOWMSG("Calling tcdrain");
        LEAVE();
        return tcdrain(file_descriptor);
    } else {
        /* ZZZ is this the correct result? */
        result = OK;
    }

    __fd_unlock(fd);

out:

    __stdio_unlock(__clib4);

    __check_abort_f(__clib4);

    RETURN(result);
    return (result);
}
