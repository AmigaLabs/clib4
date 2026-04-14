/*
 * $Id: unistd_pwrite.c,v 1.1 2022-03-04 23:09:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "../stdio/stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

ssize_t
pwrite(int fd_num, const void *buf, size_t n, off_t off) {
    struct _clib4 *__clib4 = __CLIB4;
    struct fd *fd = NULL;
    BPTR file;
    int64_t saved_pos;
    int64_t new_pos;
    int64_t file_size;
    ssize_t result = -1;

    ENTER();
    SHOWVALUE(fd_num);
    SHOWVALUE(n);
    SHOWVALUE(off);

    if (buf == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    if (n == 0) {
        result = 0;
        goto out;
    }

    __stdio_lock(__clib4);

    fd = __get_file_descriptor(__clib4, fd_num);
    if (fd == NULL || FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE)) {
        __stdio_unlock(__clib4);
        __set_errno(EBADF);
        goto out;
    }

    __fd_lock(fd);
    __stdio_unlock(__clib4);

    file = __resolve_fd_file(fd);
    if (file == BZERO) {
        __fd_unlock(fd);
        __set_errno(EBADF);
        goto out;
    }

    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_WRITE)) {
        __fd_unlock(fd);
        __set_errno(EBADF);
        goto out;
    }

    /*
     * Save the REAL file position from AmigaOS, not the cached one.
     */
    saved_pos = GetFilePosition(file);
    if (saved_pos == GETPOSITION_ERROR && IoErr() != OK) {
        __fd_unlock(fd);
        __set_errno(EIO);
        goto out;
    }

    /* Seek to the requested offset */
    if (off != saved_pos) {
        new_pos = ChangeFilePosition(file, off, OFFSET_BEGINNING);
        if (new_pos == CHANGE_FILE_ERROR) {
            /*
             * AmigaOS ChangeFilePosition fails when seeking past EOF.
             * POSIX: pwrite past EOF extends the file with zero-fill.
             * Use ChangeFileSize to extend, then retry the seek.
             */
            file_size = GetFileSize(file);
            if (file_size != GETPOSITION_ERROR && off >= file_size) {
                if (ChangeFileSize(file, off, OFFSET_BEGINNING) != -1) {
                    new_pos = ChangeFilePosition(file, off, OFFSET_BEGINNING);
                    if (new_pos == CHANGE_FILE_ERROR) {
                        ChangeFilePosition(file, saved_pos, OFFSET_BEGINNING);
                        fd->fd_Position = saved_pos;
                        __fd_unlock(fd);
                        __set_errno(EIO);
                        goto out;
                    }
                } else {
                    ChangeFilePosition(file, saved_pos, OFFSET_BEGINNING);
                    fd->fd_Position = saved_pos;
                    __fd_unlock(fd);
                    __set_errno(__translate_io_error_to_errno(IoErr()));
                    goto out;
                }
            } else {
                ChangeFilePosition(file, saved_pos, OFFSET_BEGINNING);
                fd->fd_Position = saved_pos;
                __fd_unlock(fd);
                __set_errno(EINVAL);
                goto out;
            }
        }
    }

    /* Sync the position cache so fdhookentry updates it correctly */
    fd->fd_Position = off;

    /*
     * POSIX: pwrite ignores O_APPEND.  Temporarily clear FDF_APPEND
     * to prevent fdhookentry from seeking to end of file.
     * Safe because we hold __fd_lock.
     */
    BOOL had_append = FLAG_IS_SET(fd->fd_Flags, FDF_APPEND);
    if (had_append)
        CLEAR_FLAG(fd->fd_Flags, FDF_APPEND);

    /* Use clib4's write() which goes through the optimized fdhookentry */
    result = write(fd_num, buf, n);

    if (had_append)
        SET_FLAG(fd->fd_Flags, FDF_APPEND);

    /* Restore the original file position */
    ChangeFilePosition(file, saved_pos, OFFSET_BEGINNING);

    /* Resynchronize the position cache */
    fd->fd_Position = saved_pos;

    __fd_unlock(fd);

out:

    RETURN(result);
    return result;
}
