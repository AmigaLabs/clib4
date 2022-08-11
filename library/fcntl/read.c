/*
 * $Id: fcntl_read.c,v 1.10 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

ssize_t
read(int file_descriptor, void *buffer, size_t num_bytes) {
    ssize_t num_bytes_read;
    struct fd *fd = NULL;
    ssize_t result = EOF;
    __set_errno(0);

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWPOINTER(buffer);
    SHOWVALUE(num_bytes);

    assert(buffer != NULL);
    assert((int) num_bytes >= 0);

    __check_abort();

    __stdio_lock();

    if (buffer == NULL) {
        SHOWMSG("invalid buffer");
        __set_errno(EFAULT);
        goto out;
    }

    assert(file_descriptor >= 0 && file_descriptor < __num_fd);
    assert(__fd[file_descriptor] != NULL);
    assert(FLAG_IS_SET(__fd[file_descriptor]->fd_Flags, FDF_IN_USE));

    fd = __get_file_descriptor(file_descriptor);
    if (fd == NULL) {
        __set_errno(EBADF);
        goto out;
    }

    __fd_lock(fd);

    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_READ)) {
        SHOWMSG("this descriptor is not read-enabled");

        __set_errno(EBADF);
        goto out;
    }

    if (num_bytes > 0) {
        struct file_action_message fam;

        SHOWMSG("calling the hook");

        fam.fam_Action = file_action_read;
        fam.fam_Data = buffer;
        fam.fam_Size = (int64_t) num_bytes;

        assert(fd->fd_Action != NULL);

        num_bytes_read = (*fd->fd_Action)(fd, &fam);
        if (num_bytes_read == EOF) {
            __set_errno(fam.fam_Error);
            goto out;
        }
    } else {
        num_bytes_read = 0;
    }

    result = num_bytes_read;

out:
    __fd_unlock(fd);
    __stdio_unlock();

    RETURN(result);
    return (result);
}
