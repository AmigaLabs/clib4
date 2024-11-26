/*
 * $Id: fcntl_close.c,v 1.14 2023-07-04 12:04:22 clib4devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

int
close(int file_descriptor) {
    struct file_action_message fam;
    struct fd *fd;
    int result = ERROR;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(file_descriptor);

    __check_abort_f(__clib4);

    /* We want to affect this very file descriptor and not the
       original one associated with an alias of it. */
    fd = __get_file_descriptor_dont_resolve(__clib4, file_descriptor);
    if (fd == NULL) {
        __set_errno(EBADF);
        goto out;
    }

    fam.fam_Action = file_action_close;

    assert(fd->fd_Action != NULL);

    if ((*fd->fd_Action)(__clib4, fd, &fam) < 0) {
        __set_errno(fam.fam_Error);
        goto out;
    }

    result = OK;

out:

    RETURN(result);
    return (result);
}
