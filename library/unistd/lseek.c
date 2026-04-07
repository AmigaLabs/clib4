/*
 * $Id: fcntl_lseek.c,v 1.12 2022-08-11 12:04:22 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

off_t
lseek(int file_descriptor, off_t offset, int mode) {
    struct file_action_message fam;
    off_t result = -1;
    struct fd *fd = NULL;
    off_t position;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWVALUE(offset);
    SHOWVALUE(mode);

    assert(file_descriptor >= 0 && file_descriptor < __clib4->__num_fd);
    assert(__clib4->__fd[file_descriptor] != NULL);
    assert(FLAG_IS_SET(__clib4->__fd[file_descriptor]->fd_Flags, FDF_IN_USE));

    __check_abort_f(__clib4);

    __stdio_lock(__clib4);

    fd = __get_file_descriptor(__clib4, file_descriptor);
    if (fd == NULL) {
        __set_errno(EBADF);
        goto out;
    }

    __fd_lock(fd);

    if (mode < SEEK_SET || mode > SEEK_END) {
        SHOWMSG("seek mode is invalid");

        __fd_unlock(fd);
        __set_errno(EINVAL);
        goto out;
    }

    fam.fam_Action = file_action_seek;
    fam.fam_Offset = offset;
    fam.fam_Mode = mode;

    assert(fd->fd_Action != NULL);

    /*
     * fd_Action returns the new file position on success, or EOF (-1)
     * on error with fam.fam_Error set to the errno value.
     *
     * POSIX: lseek returns (off_t)-1 on error with errno set.
     */
    position = (*fd->fd_Action)(__clib4, fd, &fam);
    if (position == EOF && fam.fam_Error != OK) {
        __fd_unlock(fd);
        __set_errno(fam.fam_Error);
        goto out;
    }

    result = position;

    __fd_unlock(fd);

out:

    __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}
