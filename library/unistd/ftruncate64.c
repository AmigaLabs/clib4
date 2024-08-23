/*
 * $Id: unistd_ftruncate64.c,v 1.0 2021-02-05 18:12:04 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
ftruncate64(int file_descriptor, _off64_t length) {
    struct ExamineData *fib = NULL;
    int result = ERROR;
    struct fd *fd = NULL;
    _off64_t initial_position = 0;
    struct _clib4 *__clib4 = __CLIB4;
    BOOL isFdLocked = FALSE;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWVALUE(length);

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
    isFdLocked = TRUE;

    if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET)) {
        __set_errno(EINVAL);
        goto out;
    }

    if (FLAG_IS_SET(fd->fd_Flags, FDF_STDIO)) {
        __set_errno(EBADF);
        goto out;
    }

    if (length < 0) {
        SHOWMSG("invalid length");

        __set_errno(EINVAL);
        goto out;
    }

    assert(FLAG_IS_SET(fd->fd_Flags, FDF_IN_USE));

    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_WRITE)) {
        SHOWMSG("file descriptor is not write-enabled");

        __set_errno(EINVAL);
        goto out;
    }

    /* Figure out how large the file is right now. */
    fib = ExamineObjectTags(EX_FileHandleInput, fd->fd_File, TAG_DONE);
    if (fib == NULL) {
        SHOWMSG("couldn't examine file");

        __set_errno(__translate_io_error_to_errno(IoErr()));
        goto out;
    }

    initial_position = (_off64_t) GetFilePosition(fd->fd_File);

    if (ChangeFileSize(fd->fd_File, length, OFFSET_BEGINNING) == CHANGE_FILE_ERROR || IoErr() != OK) {
        D(("could not reduce file to size %ld", length));

        __set_errno(__translate_io_error_to_errno(IoErr()));
        goto out;
    }

    result = OK;

out:

    /* ftruncate() may change the size of the file, but it may
	   not change the current file position. */
    ChangeFilePosition(fd->fd_File, initial_position, OFFSET_BEGINNING);

    FreeDosObject(DOS_EXAMINEDATA, fib);

    if (isFdLocked)
        __fd_unlock(fd);

    __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}
