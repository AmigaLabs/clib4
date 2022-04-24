/*
 * $Id: stat_fstat.c,v 1.11 2021-01-31 12:04:24 clib2devs Exp $
*/

#ifndef _STAT_HEADERS_H
#include "stat_headers.h"
#endif /* _STAT_HEADERS_H */

int
fstat(int file_descriptor, struct stat *buffer) {
    struct file_action_message fam;
    struct ExamineData fib;
    struct fd *fd = NULL;
    int result = ERROR;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWPOINTER(buffer);

    assert(buffer != NULL);

    if (__check_abort_enabled)
        __check_abort();

    __stdio_lock();

    if (buffer == NULL) {
        SHOWMSG("invalid buffer parameter");

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

    SHOWMSG("calling the hook");

    fam.fam_Action = file_action_examine;
    fam.fam_FileInfo = &fib;
    fam.fam_FileSystem = NULL;

    assert(fd->fd_Action != NULL);

    if ((*fd->fd_Action)(fd, &fam) < 0) {
        __set_errno(fam.fam_Error);
        goto out;
    }

    __convert_file_info_to_stat(fam.fam_FileSystem, fam.fam_FileInfo, buffer);

    result = OK;

out:

    __fd_unlock(fd);
    __stdio_unlock();

    RETURN(result);
    return (result);
}
