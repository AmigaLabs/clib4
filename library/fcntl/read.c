/*
 * $Id: fcntl_read.c,v 1.11 2023-04-06 12:04:22 clib4devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

#ifndef _SOCKET_HEADERS_H
#include "socket/socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

ssize_t
read(int file_descriptor, void *buffer, size_t num_bytes) {
    ssize_t num_bytes_read;
    struct fd *fd = NULL;
    ssize_t result = EOF;
    __set_errno(0);
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWPOINTER(buffer);
    SHOWVALUE(num_bytes);

    assert(buffer != NULL);
    assert((int) num_bytes >= 0);

    __check_abort_f(__clib4);

    __stdio_lock(__clib4);

    if (buffer == NULL) {
        SHOWMSG("invalid buffer");
        __set_errno(EFAULT);
        goto out;
    }

    assert(file_descriptor >= 0 && file_descriptor < __clib4->__num_fd);
    assert(__clib4->__fd[file_descriptor] != NULL);

    fd = __get_file_descriptor(__clib4, file_descriptor);
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
        /* Check that we are not using a socket */
        if (!FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET)) {
            struct file_action_message fam;

            SHOWMSG("calling the hook");

            fam.fam_Action = file_action_read;
            fam.fam_Data = buffer;
            fam.fam_Size = (int64_t) num_bytes;

            assert(fd->fd_Action != NULL);

            num_bytes_read = (*fd->fd_Action)(__clib4, fd, &fam);

            if (num_bytes_read == EOF) {
                __set_errno(fam.fam_Error);
                goto out;
            }
        } else {
            /* Otherwise forward the call to recv() */
            num_bytes_read = recv(file_descriptor, buffer, num_bytes, 0);
        }
    } else {
        num_bytes_read = 0;
    }

    result = num_bytes_read;

out:
    __fd_unlock(fd);
    __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}
