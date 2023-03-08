/*
 * $Id: fcntl_write.c,v 1.10 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

ssize_t
write(int file_descriptor, const void *buffer, size_t num_bytes) {
    ssize_t num_bytes_written;
    struct fd *fd = NULL;
    ssize_t result = EOF;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWPOINTER(buffer);
    SHOWVALUE(num_bytes);

    assert(buffer != NULL);
    assert((int) num_bytes >= 0);

    __check_abort();

    __stdio_lock();

    if (buffer == NULL) {
        SHOWMSG("invalid buffer address");

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

    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_WRITE)) {
        SHOWMSG("file descriptor is not write-enabled");

        __set_errno(EBADF);
        goto out;
    }

    if (num_bytes > 0) {
        struct file_action_message fam;

        SHOWMSG("calling the hook");

        fam.fam_Action = file_action_write;
        fam.fam_Data = (void *) buffer;
        fam.fam_Size = num_bytes;

        assert(fd->fd_Action != NULL);

        num_bytes_written = (*fd->fd_Action)(fd, &fam);
        if (num_bytes_written == EOF) {
            __set_errno(fam.fam_Error);
            goto out;
        }
        else if (num_bytes_written != num_bytes) {
            __set_errno(__translate_io_error_to_errno(IoErr()));
        }
    } else {
        num_bytes_written = 0;
    }

    result = num_bytes_written;

out:

    __fd_unlock(fd);

    __stdio_unlock();

    RETURN(result);
    return (result);
}
