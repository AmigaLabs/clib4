/*
 * $Id: fcntl_close.c,v 1.13 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

int
close(int file_descriptor) {
    struct file_action_message fam;
    struct fd *fd;
    int result = ERROR;

    ENTER();

    SHOWVALUE(file_descriptor);

    __check_abort();

    /* We want to affect this very file descriptor and not the
       original one associated with an alias of it. */
    fd = __get_file_descriptor_dont_resolve(file_descriptor);
    if (fd == NULL) {
        __set_errno(EBADF);
        goto out;
    }

    fam.fam_Action = file_action_close;

    assert(fd->fd_Action != NULL);

    if ((*fd->fd_Action)(fd, &fam) < 0) {
        __set_errno(fam.fam_Error);
        goto out;
    }

    result = OK;

out:

    RETURN(result);
    return (result);
}
