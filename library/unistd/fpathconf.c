/*
 * $Id: unistd_fpathconf.c,v 1.4 2024-07-04 16:36:48 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

long
fpathconf(int file_descriptor, int name) {
    struct FileHandle *fh;
    BPTR default_file;
    int error = 0;
    long ret = -1;
    struct fd *fd;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    fd = __get_file_descriptor(__clib4, file_descriptor);
    if (fd == NULL) {
        error = EINVAL;
        goto out;
    }

    if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET)) {
        error = EBADF;
        goto out;
    }

    error = __get_default_file(file_descriptor, &default_file);
    if (error != 0)
        goto out;

    fh = BADDR(default_file);

    ret = __pathconf(fh->fh_MsgPort, name);

out:

    if (ret == -1 && error != 0)
        __set_errno(error);

    RETURN(ret);
    return (ret);
}
