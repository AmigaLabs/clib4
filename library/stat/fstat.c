/*
 * $Id: stat_fstat.c,v 1.11 2021-01-31 12:04:24 clib4devs Exp $
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
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWPOINTER(buffer);

    assert(buffer != NULL);

    __check_abort_f(__clib4);

    __stdio_lock(__clib4);

    if (buffer == NULL) {
        SHOWMSG("invalid buffer parameter");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    fd = __get_file_descriptor(__clib4, file_descriptor);
    if (fd == NULL) {
        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    assert(FLAG_IS_SET(__clib4->__fd[file_descriptor]->fd_Flags, FDF_IN_USE));

    __fd_lock(fd);

    SHOWMSG("calling the hook");

    fam.fam_Action = file_action_examine;
    fam.fam_FileInfo = &fib;
    fam.fam_FileSystem = NULL;

    assert(fd->fd_Action != NULL);

    if ((*fd->fd_Action)(__clib4, fd, &fam) < 0) {
        __set_errno_r(__clib4, fam.fam_Error);
        goto out;
    }

    __convert_file_info_to_stat(fam.fam_FileSystem, fam.fam_FileInfo, buffer);

    if (fam.fam_FileInfo->Type != ST_CONSOLE) {
        /* Close ExamineObjectTag object created when fd->fd_Action is executed  */
        FreeDosObject(DOS_EXAMINEDATA, fam.fam_FileInfo);
    }
    else {
        /* If ExamineObjectTag was failed we have to free the dummy ExamineData structure created */
        __free_r(__clib4, fam.fam_FileInfo);
    }

    result = OK;

out:

    __fd_unlock(fd);
    __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}
