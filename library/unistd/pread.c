/*
 * $Id: unistd_pread.c,v 1.0 2021-02-21 23:05:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "../stdio/stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

ssize_t
pread(int fd_num, void *buf, size_t n, off_t off) {
    struct _clib4 *__clib4 = __CLIB4;
    struct fd *fd = NULL;
    BPTR file;
    int64_t saved_pos;
    int64_t new_pos;
    ssize_t num_read;

    if (buf == NULL) {
        __set_errno(EFAULT);
        return -1;
    }

    if (n == 0)
        return 0;

    __stdio_lock(__clib4);

    fd = __get_file_descriptor(__clib4, fd_num);
    if (fd == NULL || FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE)) {
        __stdio_unlock(__clib4);
        __set_errno(EBADF);
        return -1;
    }

    __fd_lock(fd);
    __stdio_unlock(__clib4);

    file = __resolve_fd_file(fd);
    if (file == BZERO) {
        __fd_unlock(fd);
        __set_errno(EBADF);
        return -1;
    }

    /*
     * Save the REAL file position from AmigaOS, not the cached one.
     * This avoids desynchronization between fd->fd_Position and the
     * actual file handle position (which can happen when ExamineObjectTags
     * or __grow_file_size move the file pointer without updating the cache).
     */
    saved_pos = GetFilePosition(file);
    if (saved_pos == GETPOSITION_ERROR && IoErr() != OK) {
        __fd_unlock(fd);
        __set_errno(EIO);
        return -1;
    }

    /* Seek to the requested offset */
    if (off != saved_pos) {
        new_pos = ChangeFilePosition(file, off, OFFSET_BEGINNING);
        if (new_pos == CHANGE_FILE_ERROR) {
            /*
             * POSIX: pread at or past EOF must return 0, not an error.
             * Check if offset is at or past end of file.
             */
            int64_t file_size = GetFileSize(file);
            if (file_size != GETPOSITION_ERROR && off >= file_size) {
                /* Restore position */
                ChangeFilePosition(file, saved_pos, OFFSET_BEGINNING);
                fd->fd_Position = saved_pos;
                __fd_unlock(fd);
                return 0;
            }
            /* Real seek error — restore position */
            ChangeFilePosition(file, saved_pos, OFFSET_BEGINNING);
            fd->fd_Position = saved_pos;
            __fd_unlock(fd);
            __set_errno(EINVAL);
            return -1;
        }
    }

    /* Sync the position cache so fdhookentry updates it correctly */
    fd->fd_Position = off;

    /* Use clib4's read() which goes through the optimized fdhookentry */
    num_read = read(fd_num, buf, n);

    /* Restore the original file position */
    ChangeFilePosition(file, saved_pos, OFFSET_BEGINNING);

    /* Resynchronize the position cache */
    fd->fd_Position = saved_pos;

    __fd_unlock(fd);
    return num_read;
}
