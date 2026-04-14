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

    if (__builtin_expect(buffer == NULL, 0)) {
        SHOWMSG("invalid buffer address");
        __set_errno(EFAULT);
        goto out;
    }

    assert(file_descriptor >= 0 && file_descriptor < __clib4->__num_fd);
    assert(__clib4->__fd[file_descriptor] != NULL);

    /* Inline fd lookup — avoid __get_file_descriptor (takes global lock) */
    if (__builtin_expect(file_descriptor < 0 || file_descriptor >= __clib4->__num_fd, 0)) {
        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    fd = __clib4->__fd[file_descriptor];
    if (__builtin_expect(fd == NULL, 0)) {
        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    /* Resolve alias */
    if (__builtin_expect(fd->fd_Original != NULL, 0))
        fd = fd->fd_Original;

    if (__builtin_expect(FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE), 0)) {
        if (FLAG_IS_SET(fd->fd_Flags, FDF_PIPE)) {
            __set_errno(EPIPE);
        } else {
            __set_errno_r(__clib4, EBADF);
        }
        goto out;
    }

    if (__builtin_expect(FLAG_IS_CLEAR(fd->fd_Flags, FDF_WRITE), 0)) {
        SHOWMSG("file descriptor is not write-enabled");
        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    if (__builtin_expect(num_bytes == 0, 0)) {
        result = 0;
        goto out;
    }

    /* Check that we are not using a socket */
    if (__builtin_expect(!FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET), 1)) {
        struct file_action_message fam;

        SHOWMSG("calling the hook");

        fam.fam_Action = file_action_write;
        fam.fam_Data = (void *) buffer;
        fam.fam_Size = num_bytes;

        assert(fd->fd_Action != NULL);

        num_bytes_written = (*fd->fd_Action)(__clib4, fd, &fam);
        if (__builtin_expect(num_bytes_written == EOF, 0)) {
            __set_errno_r(__clib4, fam.fam_Error);
            goto out;
        } else if (__builtin_expect(num_bytes_written != (ssize_t) num_bytes, 0)) {
            __set_errno_r(__clib4, __translate_io_error_to_errno(IoErr()));
        }
    } else {
        /* Otherwise forward the call to send() */
        num_bytes_written = __send_r(__clib4, file_descriptor, buffer, num_bytes, 0);
    }

    result = num_bytes_written;

out:

    RETURN(result);
    return (result);
}