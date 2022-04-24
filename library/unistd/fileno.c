/*
 * $Id: unistd_fileno.c,v 1.7 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
fileno(FILE *file) {
    struct iob *iob = (struct iob *) file;
    int result = ERROR;

    ENTER();

    SHOWPOINTER(file);

    assert(file != NULL);

    flockfile(file);

    if (file == NULL) {
        SHOWMSG("invalid file parameter");

        __set_errno(EFAULT);
        goto out;
    }

    assert(__is_valid_iob(iob));
    assert(FLAG_IS_SET(iob->iob_Flags, IOBF_IN_USE));

    if (FLAG_IS_CLEAR(iob->iob_Flags, IOBF_IN_USE)) {
        __set_errno(EBADF);
        goto out;
    }

    assert(iob->iob_Descriptor >= 0 && iob->iob_Descriptor < __num_fd);
    assert(__fd[iob->iob_Descriptor] != NULL);
    assert(FLAG_IS_SET(__fd[iob->iob_Descriptor]->fd_Flags, FDF_IN_USE));

    result = iob->iob_Descriptor;

out:

    funlockfile(file);

    RETURN(result);
    return (result);
}
