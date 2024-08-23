/*
 * $Id: uio_readv.c,v 1.6 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _UIO_HEADERS_H
#include "uio_headers.h"
#endif /* _UIO_HEADERS_H */

ssize_t
readv(int file_descriptor, const struct iovec *iov, int vec_count) {
    ssize_t result = EOF;
    struct file_action_message msg;
    ssize_t total_num_bytes_read;
    ssize_t part_num_bytes_read;
    ssize_t num_bytes_read;
    struct fd *fd = NULL;
    int i;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWPOINTER(iov);
    SHOWVALUE(vec_count);

    __stdio_lock(__clib4);

    if (iov == NULL) {
        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    if (vec_count < 1 || vec_count > MAX_IOVEC) {
        __set_errno_r(__clib4, EINVAL);
        goto out;
    }

    /* Check for overflow. An expensive test, but better to do it here than in the read loop. */
    for (i = 0, total_num_bytes_read = 0; i < vec_count; i++) {
        total_num_bytes_read += iov[i].iov_len;
        if (total_num_bytes_read < 0) /* Rollover. */
        {
            __set_errno_r(__clib4, EINVAL);
            goto out;
        }
    }

    fd = __get_file_descriptor(__clib4, file_descriptor);
    if (fd == NULL) {
        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    __fd_lock(fd);

    total_num_bytes_read = 0;
    part_num_bytes_read = 0;

    i = 0;

    while (i < vec_count) /* XXX: Should this loop be locked? */
    {
        __check_abort_f(__clib4);

        if (iov[i].iov_len > 0) {
            msg.fam_Action = file_action_read;
            msg.fam_Data = (char *) iov[i].iov_base + part_num_bytes_read;
            msg.fam_Size = iov[i].iov_len - part_num_bytes_read;

            num_bytes_read = (*fd->fd_Action)(__clib4, fd, &msg);
            if (num_bytes_read == EOF) {
                __set_errno_r(__clib4, msg.fam_Error);
                goto out;
            }

            /* End of file reached? */
            if (num_bytes_read == 0)
                break;

            total_num_bytes_read += num_bytes_read;

            part_num_bytes_read += num_bytes_read;
            if ((size_t) part_num_bytes_read < iov[i].iov_len) {
                /* Avoid busy-waiting for more data here? */
                continue;
            }
        }

        part_num_bytes_read = 0;

        i++;
    }

    result = total_num_bytes_read;

out:

    __fd_unlock(fd);
    __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}
