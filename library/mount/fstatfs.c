/*
 * $Id: mount_fstatfs.c,v 1.15 2023-07-04 14:39:23 clib4devs Exp $
*/

#ifndef _MOUNT_HEADERS_H
#include "mount_headers.h"
#endif /* _MOUNT_HEADERS_H */

int
fstatfs(int file_descriptor, struct statfs *buf) {
    D_S(struct InfoData, id);
    BPTR parent_dir = BZERO;
    int result = ERROR;
    struct fd *fd = NULL;
    LONG success;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWPOINTER(buf);

    assert(buf != NULL);

    __check_abort_f(__clib4);

    __stdio_lock(__clib4);

    if (buf == NULL) {
        SHOWMSG("invalid buffer parameter");

        __set_errno_r(__clib4, EFAULT);
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

    if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET)) {
        __set_errno_r(__clib4, EINVAL);
        goto out;
    }

    if (FLAG_IS_SET(fd->fd_Flags, FDF_STDIO)) {
        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    parent_dir = ParentOfFH(fd->fd_File);
    if (parent_dir == BZERO) {
        SHOWMSG("couldn't find parent directory");

        __set_errno_r(__clib4, __translate_io_error_to_errno(IoErr()));
        goto out;
    }

    success = Info(parent_dir, id);
    if (NO success) {
        SHOWMSG("couldn't get info on drive");

        __set_errno_r(__clib4, __translate_io_error_to_errno(IoErr()));
        goto out;
    }

    __convert_info_to_statfs(id, buf);
    result = OK;

out:
    __fd_unlock(fd);
    __stdio_unlock(__clib4);
    UnLock(parent_dir);

    RETURN(result);
    return (result);
}
