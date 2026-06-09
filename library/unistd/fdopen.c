/*
 * $Id: unistd_fdopen.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * fdopen() — newlib-inspired rewrite using __sfp() for stream allocation.
 */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

FILE *
fdopen(int file_descriptor, const char *type) {
    struct _clib4 *__clib4 = __CLIB4;
    struct iob *fp;
    struct SignalSemaphore *lock;
    ULONG file_flags = 0;
    int open_mode;
    struct fd *fd;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWSTRING(type);

    if (type == NULL) {
        __set_errno(EFAULT);
        RETURN(NULL);
        return NULL;
    }

    /* Validate the file descriptor. */
    fd = __get_file_descriptor(__clib4, file_descriptor);
    if (fd == NULL || FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE)) {
        __set_errno(EBADF);
        RETURN(NULL);
        return NULL;
    }

    CHECK_INIT(__clib4);

    /* Parse the mode string. */
    switch (type[0]) {
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
            __set_errno(EINVAL);
            RETURN(NULL);
            return NULL;
    }

    if ((type[1] == '+') || (type[1] != '\0' && type[2] == '+')) {
        CLEAR_FLAG(open_mode, O_RDONLY);
        CLEAR_FLAG(open_mode, O_WRONLY);
        SET_FLAG(open_mode, O_RDWR);
    } else if (type[1] != '\0' && type[1] == 'b' && type[2] == 'l') {
        SET_FLAG(open_mode, O_LITTLE_ENDIAN);
        SET_FLAG(file_flags, IOBF_LITTLE_ENDIAN);
    }

    /* Allocate a free FILE slot from the glue list. */
    __stdio_lock(__clib4);
    fp = __sfp(__clib4);
    __stdio_unlock(__clib4);

    if (fp == NULL) {
        RETURN(NULL);
        return NULL;
    }

    /* Create a lock for this stream. */
    lock = __create_semaphore();
    if (lock == NULL) {
        CLEAR_FLAG(fp->iob_Flags, IOBF_IN_USE);
        __set_errno(ENOMEM);
        RETURN(NULL);
        return NULL;
    }

    /* Update the append flag on the fd layer. */
    if (FLAG_IS_SET(open_mode, O_APPEND))
        SET_FLAG(fd->fd_Flags, FDF_APPEND);

    /* Set up FILE flags. */
    file_flags |= IOBF_IN_USE | IOBF_NO_NUL;
    if (FLAG_IS_SET(open_mode, O_RDONLY) || FLAG_IS_SET(open_mode, O_RDWR))
        SET_FLAG(file_flags, IOBF_READ);
    if (FLAG_IS_SET(open_mode, O_WRONLY) || FLAG_IS_SET(open_mode, O_RDWR))
        SET_FLAG(file_flags, IOBF_WRITE);
    if (FLAG_IS_SET(open_mode, O_APPEND))
        SET_FLAG(file_flags, IOBF_APP);

    fp->iob_Flags = file_flags;
    fp->iob_Flags2 = 0;
    memset(&fp->iob_mbState, 0, sizeof(_mbstate_t));

    /* Lazy buffer allocation — clear buffer state. */
    fp->iob_Buffer = NULL;
    fp->iob_BufferSize = 0;
    fp->iob_BufferPosition = 0;
    fp->iob_BufferReadBytes = 0;
    fp->iob_BufferWriteBytes = 0;
    fp->iob_CustomBuffer = NULL;
    fp->iob_isVBuffer = FALSE;

    /* Set up function pointers. */
    fp->_read = __sread;
    fp->_write = __swrite;
    fp->_seek = __sseek;
    fp->_close = __sclose;
    fp->_seek64 = __sseek;
    fp->_cookie = fp;

    fp->iob_Descriptor = file_descriptor;
    fp->iob_Lock = lock;
    fp->iob_TaskLock = NULL;

    /* Transition: also set up legacy fields. */
    fp->iob_Action = __iob_hook_entry;
    fp->iob_SlotNumber = -1;

    fp->_blksize = BUFSIZ;
    fp->_ub._base = NULL;
    fp->_ub._size = 0;
    fp->_lb._base = NULL;
    fp->_lb._size = 0;
    fp->_offset = 0;

    RETURN((FILE *) fp);
    return (FILE *) fp;
}
