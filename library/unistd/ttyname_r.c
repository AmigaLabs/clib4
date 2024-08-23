/*
 * $Id: unistd_ttyname_r.c,v 1.7 2023-03-30 14:39:23 clib4devs Exp $
*/

#ifndef    _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
ttyname_r(int file_descriptor, char *name, size_t buflen) {
    const char *tty_file_name;
    struct fd *fd;
    int result;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(file_descriptor);

    __stdio_lock(__clib4);

    fd = __get_file_descriptor(__clib4, file_descriptor);
    if (fd == NULL) {
        result = EBADF;
        goto out;
    }

    __fd_lock(fd);

    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_IS_INTERACTIVE) && FLAG_IS_CLEAR(fd->fd_Flags, FDF_IS_SERIAL)) {
        BPTR file;

        file = __resolve_fd_file(fd);
        if (file == BZERO || NOT IsInteractive(file)) {
            __fd_unlock(fd);
            result = ENOTTY;
            goto out;
        }
    }

    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_IS_SERIAL)) {
        if (__clib4->__unix_path_semantics)
            tty_file_name = "/CONSOLE";
        else
            tty_file_name = "CONSOLE:";
    }
    else {
        if (__clib4->__unix_path_semantics)
            tty_file_name = "/SER";
        else
            tty_file_name = "SER:";
    }

    if (buflen < strlen(tty_file_name) + 1) { /* XXX Should this be _POSIX_PATH_MAX? */
        __fd_unlock(fd);
        result = ERANGE;
        goto out;
    }

    strcpy(name, tty_file_name);

    result = OK;

    __fd_unlock(fd);

out:

    __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}
