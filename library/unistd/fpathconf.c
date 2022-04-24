/*
 * $Id: unistd_fpathconf.c,v 1.3 2006-09-17 16:36:48 clib2devs Exp $
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

    ENTER();

    fd = __get_file_descriptor(file_descriptor);
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

    ret = __pathconf(fh->fh_Type, name);

out:

    if (ret == -1 && error != 0)
        __set_errno(error);

    RETURN(ret);
    return (ret);
}
