/*
 * $Id: unistd_fileno.c,v 1.7 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
fileno(FILE *file) {
    struct iob *iob = (struct iob *) file;
    int result = ERROR;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWPOINTER(file);

    assert(file != NULL);

    if (file == NULL) {
        SHOWMSG("invalid file parameter");
        __set_errno(EFAULT);

        RETURN(result);
        return (result);
    }

    __flockfile_r(__clib4, file);

    assert(__is_valid_iob(__clib4, iob));
    assert(FLAG_IS_SET(iob->iob_Flags, IOBF_IN_USE));

    if (FLAG_IS_CLEAR(iob->iob_Flags, IOBF_IN_USE)) {
        __set_errno(EBADF);
        goto out;
    }

    assert(iob->iob_Descriptor >= 0 && iob->iob_Descriptor < __clib4->__num_fd);
    assert(__clib4->__fd[iob->iob_Descriptor] != NULL);
    assert(FLAG_IS_SET(__clib4->__fd[iob->iob_Descriptor]->fd_Flags, FDF_IN_USE));

    result = iob->iob_Descriptor;

out:

    __funlockfile_r(__clib4, file);

    RETURN(result);
    return (result);
}
