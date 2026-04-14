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
__read_internal(struct _clib4 *__clib4, int file_descriptor, void *buffer, size_t num_bytes) {
    ssize_t num_bytes_read;
    struct fd *fd = NULL;
    ssize_t result = EOF;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWPOINTER(buffer);
    SHOWVALUE(num_bytes);

    assert(buffer != NULL);
    assert((int) num_bytes >= 0);

    if (__builtin_expect(buffer == NULL, 0)) {
        SHOWMSG("invalid buffer");
        __set_errno(EFAULT);
        goto out;
    }

    assert(file_descriptor >= 0 && file_descriptor < __clib4->__num_fd);
    assert(__clib4->__fd[file_descriptor] != NULL);

    if (__builtin_expect(file_descriptor < 0 || file_descriptor >= __clib4->__num_fd, 0)) {
        __set_errno(EBADF);
        goto out;
    }

    fd = __clib4->__fd[file_descriptor];
    if (__builtin_expect(fd == NULL, 0)) {
        __set_errno(EBADF);
        goto out;
    }

    /* Resolve alias */
    if (__builtin_expect(fd->fd_Original != NULL, 0))
        fd = fd->fd_Original;

    if (__builtin_expect(FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE), 0)) {
        if (FLAG_IS_SET(fd->fd_Flags, FDF_PIPE)) {
            __set_errno(EPIPE);
        } else {
            __set_errno(EBADF);
        }
        goto out;
    }

    if (__builtin_expect(FLAG_IS_CLEAR(fd->fd_Flags, FDF_READ), 0)) {
        SHOWMSG("this descriptor is not read-enabled");
        __set_errno(EBADF);
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

        fam.fam_Action = file_action_read;
        fam.fam_Data = buffer;
        fam.fam_Size = (int64_t) num_bytes;

        assert(fd->fd_Action != NULL);

        num_bytes_read = (*fd->fd_Action)(__clib4, fd, &fam);

        if (__builtin_expect(num_bytes_read == EOF, 0)) {
            __set_errno(fam.fam_Error);
            goto out;
        }
    } else {
        /* Otherwise forward the call to recv() */
        num_bytes_read = recv(file_descriptor, buffer, num_bytes, 0);
    }

    result = num_bytes_read;

out:

    RETURN(result);
    return (result);
}

static void byteswap16(void *ptr) {
    uint8_t *b = ptr;
    uint8_t t = b[0]; b[0] = b[1]; b[1] = t;
}

static void byteswap32(void *ptr) {
    uint8_t *b = ptr;
    uint8_t t;
    t = b[0]; b[0] = b[3]; b[3] = t;
    t = b[1]; b[1] = b[2]; b[2] = t;
}

static void byteswap64(void *ptr) {
    uint8_t *b = ptr;
    for (int i = 0; i < 4; ++i) {
        uint8_t t = b[i];
        b[i] = b[7 - i];
        b[7 - i] = t;
    }
}

ssize_t
read(int file_descriptor, void *buffer, size_t num_bytes) {
    struct _clib4 *__clib4 = __CLIB4;
    ssize_t ret = __read_internal(__clib4, file_descriptor, buffer, num_bytes);
    if (ret != (ssize_t) num_bytes)
        return ret; // return partial or failed read unchanged

    /* Little-endian byte swap for small fixed-size reads.
     * Inline the fd lookup to avoid a second __get_file_descriptor call. */
    if (__builtin_expect(num_bytes == 2 || num_bytes == 4 || num_bytes == 8, 0)) {
        struct fd *fd = NULL;
        if (file_descriptor >= 0 && file_descriptor < __clib4->__num_fd)
            fd = __clib4->__fd[file_descriptor];
        if (fd != NULL && fd->fd_Original != NULL)
            fd = fd->fd_Original;
        if (fd != NULL && !FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET) && FLAG_IS_SET(fd->fd_Flags, FDF_LITTLE_ENDIAN)) {
            SHOWMSG("[read] Reading in Little endian mode\n");
            if (num_bytes == 2) {
                byteswap16(buffer);
            } else if (num_bytes == 4) {
                byteswap32(buffer);
            } else if (num_bytes == 8) {
                byteswap64(buffer);
            }
        }
    }

    return ret;
}