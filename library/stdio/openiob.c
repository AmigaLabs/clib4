/*
 * $Id: stdio_openiob.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * __open_iob() — legacy entry point, updated for the new I/O layer.
 * Still used by code that opens files via the old __iob[] slot system
 * (e.g. stdin/stdout/stderr init, freopen via old path).
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

int
__open_iob(struct _clib4 *__clib4, const char *filename, const char *mode, int file_descriptor, int slot_number) {
    struct SignalSemaphore *lock;
    ULONG file_flags = 0;
    int result = ERROR;
    int open_mode;
    struct fd *fd = NULL;
    struct iob *file;

    ENTER();

    SHOWSTRING(filename);
    SHOWSTRING(mode);
    SHOWVALUE(slot_number);

    assert(mode != NULL && 0 <= slot_number && slot_number < __clib4->__num_iob);

    file = __clib4->__iob[slot_number];

    assert(FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE));

    /* Figure out if the file descriptor provided is any use. */
    if (file_descriptor >= 0) {
        fd = __get_file_descriptor(__clib4, file_descriptor);
        if (fd == NULL) {
            __set_errno_r(__clib4, EBADF);
            goto out;
        }
    }

    /* The first character selects the access mode. */
    switch (mode[0]) {
        case 'r':
            open_mode = O_RDONLY;
            break;
        case 'w':
            open_mode = O_WRONLY | O_CREAT | O_TRUNC;
            break;
        case 'a':
            open_mode = O_WRONLY | O_CREAT | O_APPEND;
            break;
        default:
            __set_errno_r(__clib4, EINVAL);
            goto out;
    }

    /* If the second or third character is a '+', switch to read/write mode. */
    if ((mode[1] == '+') || (mode[1] != '\0' && mode[2] == '+')) {
        CLEAR_FLAG(open_mode, O_RDONLY);
        CLEAR_FLAG(open_mode, O_WRONLY);
        SET_FLAG(open_mode, O_RDWR);
    } else if (mode[1] != '\0' && mode[1] == 'b' && mode[2] == 'l') {
        SET_FLAG(open_mode, O_LITTLE_ENDIAN);
        SET_FLAG(file_flags, IOBF_LITTLE_ENDIAN);
    }

    if (file_descriptor < 0) {
        assert(filename != NULL);

        file_descriptor = __open_r(__clib4, filename, open_mode);
        if (file_descriptor < 0) {
            SHOWMSG("couldn't open the file");
            goto out;
        }
    } else {
        /* Update the append flag. */
        if (FLAG_IS_SET(open_mode, O_APPEND))
            SET_FLAG(fd->fd_Flags, FDF_APPEND);
        else
            CLEAR_FLAG(fd->fd_Flags, FDF_APPEND);
    }

    /* Allocate a lock for this stream. */
    lock = __create_semaphore();
    if (lock == NULL)
        goto out;

    /* Set up the FILE flags. */
    file_flags |= IOBF_IN_USE | IOBF_NO_NUL;

    if (FLAG_IS_SET(open_mode, O_RDONLY) || FLAG_IS_SET(open_mode, O_RDWR))
        SET_FLAG(file_flags, IOBF_READ);
    if (FLAG_IS_SET(open_mode, O_WRONLY) || FLAG_IS_SET(open_mode, O_RDWR))
        SET_FLAG(file_flags, IOBF_WRITE);
    if (FLAG_IS_SET(open_mode, O_APPEND))
        SET_FLAG(file_flags, IOBF_APP);

    /* Initialize via __initialize_iob (sets legacy fields). */
    __initialize_iob(file,
                     __iob_hook_entry,
                     NULL,          /* No custom buffer — lazy allocation */
                     NULL,          /* No buffer yet */
                     (int64_t) 0,   /* Buffer size 0 — will be set by __smakebuf */
                     file_descriptor,
                     slot_number,
                     file_flags,
                     lock);

    /* Set up the new function pointer I/O path. */
    file->_read = __sread;
    file->_write = __swrite;
    file->_seek = __sseek;
    file->_close = __sclose;
    file->_seek64 = __sseek;
    file->_cookie = file;

    file->_blksize = BUFSIZ;

    result = OK;

out:

    RETURN(result);
    return result;
}
