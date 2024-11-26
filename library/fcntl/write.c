/*
 * $Id: fcntl_write.c,v 1.13 2024-07-12 12:04:22 clib4devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

#ifndef _SOCKET_HEADERS_H
#include "socket/socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

ssize_t
write(int file_descriptor, const void *buffer, size_t num_bytes) {
    return __write_r(__CLIB4, file_descriptor, buffer, num_bytes);
}

ssize_t
__write_r(struct _clib4 *__clib4, int file_descriptor, const void *buffer, size_t num_bytes) {
    ssize_t num_bytes_written;
    struct fd *fd = NULL;
    ssize_t result = EOF;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWPOINTER(buffer);
    SHOWVALUE(num_bytes);

    assert(buffer != NULL);
    assert((int) num_bytes >= 0);

    __stdio_lock(__clib4);

    if (buffer == NULL) {
        SHOWMSG("invalid buffer address");

        __set_errno(EFAULT);
        goto out;
    }

    assert(file_descriptor >= 0 && file_descriptor < __clib4->__num_fd);
    assert(__clib4->__fd[file_descriptor] != NULL);
    assert(FLAG_IS_SET(__clib4->__fd[file_descriptor]->fd_Flags, FDF_IN_USE));

    fd = __get_file_descriptor(__clib4, file_descriptor);
    if (fd == NULL) {
        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    __fd_lock(fd);

    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE) && FLAG_IS_SET(fd->fd_Flags, FDF_PIPE)) {
        SHOWMSG("file descriptor is a closed PIPE");

        __set_errno(SIGPIPE);
        goto out;
    }

    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_WRITE)) {
        SHOWMSG("file descriptor is not write-enabled");

        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    if (num_bytes > 0) {
        /* Check that we are not using a socket */
        if (!FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET)) {
            struct file_action_message fam;

            SHOWMSG("calling the hook");

            fam.fam_Action = file_action_write;
            fam.fam_Data = (void *) buffer;
            fam.fam_Size = num_bytes;

            assert(fd->fd_Action != NULL);

            num_bytes_written = (*fd->fd_Action)(__clib4, fd, &fam);
            if (num_bytes_written == EOF) {
                __set_errno_r(__clib4, fam.fam_Error);
                goto out;
            } else if (num_bytes_written != num_bytes) {
                __set_errno_r(__clib4, __translate_io_error_to_errno(IoErr()));
            }
        } else {
            /* Otherwise forward the call to send() */
            num_bytes_written = __send_r(__clib4, file_descriptor, buffer, num_bytes, 0);
        }
    } else {
        num_bytes_written = 0;
    }

    result = num_bytes_written;

out:

    __fd_unlock(fd);
    __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}