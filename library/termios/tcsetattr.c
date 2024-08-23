/*
 * $Id: termios_tcsetattr.c,v 1.5 2006-11-16 14:39:23 clib4devs Exp $
*/

#ifndef _TERMIOS_HEADERS_H
#include "termios_headers.h"
#endif

static int
set_console_termios(struct fd *fd, struct termios *new_tios) {
    struct termios *old_tios;
    int result = ERROR;
    BPTR file;

    /* TODO: Check for some "impossible" combinations here? */

    old_tios = __get_termios(fd);
    if (old_tios == NULL)
        goto out;

    if (old_tios->type != TIOST_CONSOLE)
        goto out;

    file = __resolve_fd_file(fd);
    if (file == BZERO)
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
    struct _clib4 *__clib4 = __CLIB4;
    BOOL isFdLocked = FALSE;
    BOOL isStdioLocked = FALSE;

    ENTER();

    __stdio_lock(__clib4);
    isStdioLocked = TRUE;

    if (tios == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    fd = __get_file_descriptor(__clib4, file_descriptor);
    if (fd == NULL) {
        SHOWMSG("tcsetattr() was not called with a file descriptor.\n");

        __set_errno(EBADF);
        goto out;
    }

    __fd_lock(fd);
    isFdLocked = TRUE;

    /* The following is in case the termios structure was manually constructed. (it should have been zero:ed in that case)  */
    if (tios->type == TIOST_INVALID) {
        __stdio_unlock(__clib4);
        isStdioLocked = FALSE;
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
        goto out;
    } else if (tios->type == TIOST_CONSOLE) {
        if (how == TCSADRAIN || how == TCSAFLUSH) {
            __stdio_unlock(__clib4);
            isStdioLocked = FALSE;
            __fd_unlock(fd);
            isFdLocked = FALSE;
            switch (how) {
                case TCSADRAIN:
                    if (tcdrain(file_descriptor) != OK) {
                        goto out;
                    }
                    break;
                case TCSAFLUSH:
                    if (tcflush(file_descriptor, TCIOFLUSH) != OK) {
                        goto out;
                    }
                    break;
            }
        }

        if (set_console_termios(fd, tios) != OK) {
            __set_errno(EIO);
            goto out;
        }
    } else {
        __set_errno(ENOTTY);
        goto out;
    }

    result = OK;

out:

    if (isFdLocked)
        __fd_unlock(fd);

    if (isStdioLocked)
        __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}
