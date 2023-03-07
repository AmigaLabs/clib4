/*
 * $Id: termios_tcsetattr.c,v 1.5 2006-11-16 14:39:23 clib2devs Exp $
*/

#ifndef _TERMIOS_HEADERS_H
#include "termios_headers.h"
#endif

static int
set_console_termios(struct fd *fd, struct termios *new_tios) {
    struct termios *old_tios;
    int result = ERROR;
    BPTR file;
    BOOL use_ncurses = FALSE;

    if (FLAG_IS_SET(new_tios->c_cflag, NCURSES))
        use_ncurses = TRUE;

    /* TODO: Check for some "impossible" combinations here? */

    old_tios = __get_termios(fd, use_ncurses);
    if (old_tios == NULL)
        goto out;

    if (old_tios->type != TIOST_CONSOLE)
        goto out;

    file = __resolve_fd_file(fd);
    if (file == ZERO)
        goto out;

    if (FLAG_IS_CLEAR(new_tios->c_lflag, ICANON)) {
        SetMode(file, DOSTRUE); /* Set Raw = Non-Canonical mode. */
        SET_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
    } else {
        SetMode(file, DOSFALSE); /* Set Cooked = Canonical mode. */
        CLEAR_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
    }

    /* Most of the processing (except raw/cooked mode switch) is handled in the hook. */
    memcpy(old_tios, new_tios, offsetof(struct termios,type));

    result = OK;

out:

    return (result);
}

int
tcsetattr(int file_descriptor, int how, struct termios *tios) {
    int result = ERROR;
    struct fd *fd = NULL;
    struct termios new_tios;
    int type;

    __stdio_lock();

    if (tios == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    fd = __get_file_descriptor(file_descriptor);
    if (fd == NULL) {
        SHOWMSG("tcsetattr() was not called with a file descriptor.\n");

        __set_errno(EBADF);
        goto out;
    }

    __fd_lock(fd);

    /* The following is in case the termios structure was manually constructed. (it should have been zero:ed in that case)  */
    if (tios->type == TIOST_INVALID) {
        if (tcgetattr(file_descriptor, &new_tios) != OK)
            goto out; /* Pass errno from tcgetattr() */

        type = new_tios.type;

        memcpy(&new_tios, tios, sizeof(struct termios));

        new_tios.c_ispeed = B9600;
        new_tios.c_ospeed = B9600;

        new_tios.type = type;

        tios = &new_tios;
    }

    if (tios->type == TIOST_SERIAL) {
        __set_errno(ENXIO);    /* Unimplemented (for now). */
        __fd_unlock(fd);
        goto out;
    } else if (tios->type == TIOST_CONSOLE) {
        if (how == TCSADRAIN) {
            if (tcdrain(file_descriptor) != OK) {
                __fd_unlock(fd);
                goto out;
            }
        } else if (how == TCSAFLUSH) {
            if (tcflush(file_descriptor, TCIOFLUSH) != OK) {
                __fd_unlock(fd);
                goto out;
            }
        }

        if (set_console_termios(fd, tios) != OK) {
            __set_errno(EIO);
            __fd_unlock(fd);
            goto out;
        }
    } else {
        __set_errno(ENOTTY);
        __fd_unlock(fd);
        goto out;
    }

    result = OK;

    __fd_unlock(fd);

out:

    __stdio_unlock();

    return (result);
}
