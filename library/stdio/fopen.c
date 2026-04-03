/*
 * $Id: stdio_fopen.c,v 1.7 2025-01-01 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

FILE *
fopen(const char *filename, const char *mode) {
    struct _clib4 *__clib4 = __CLIB4;
    struct iob *fp;
    struct SignalSemaphore *lock;
    int open_mode;
    int file_descriptor;
    ULONG file_flags = 0;

    ENTER();

    SHOWSTRING(filename);
    SHOWSTRING(mode);

    assert(filename != NULL && mode != NULL);

    if (filename == NULL || mode == NULL) {
        SHOWMSG("invalid parameters");
        __set_errno_r(__clib4, EFAULT);
        RETURN(NULL);
        return NULL;
    }

    CHECK_INIT(__clib4);

    /* Parse the mode string to determine open flags */
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
            RETURN(NULL);
            return NULL;
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

    /* Get a free FILE slot from the glue list */
    __stdio_lock(__clib4);
    fp = __sfp(__clib4);
    __stdio_unlock(__clib4);

    if (fp == NULL) {
        RETURN(NULL);
        return NULL;
    }

    /* Open the file to get an fd */
    file_descriptor = __open_r(__clib4, filename, open_mode);
    if (file_descriptor < 0) {
        SHOWMSG("couldn't open the file");
        /* Release the FILE slot */
        CLEAR_FLAG(fp->iob_Flags, IOBF_IN_USE);
        RETURN(NULL);
        return NULL;
    }

    /* Create a lock for this stream */
    lock = __create_semaphore();
    if (lock == NULL) {
        close(file_descriptor);
        CLEAR_FLAG(fp->iob_Flags, IOBF_IN_USE);
        __set_errno(ENOMEM);
        RETURN(NULL);
        return NULL;
    }

    /* Set up the FILE flags */
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

    /* Buffer is NOT allocated here — lazy allocation via __smakebuf() on first I/O */
    fp->iob_Buffer = NULL;
    fp->iob_BufferSize = 0;
    fp->iob_BufferPosition = 0;
    fp->iob_BufferReadBytes = 0;
    fp->iob_BufferWriteBytes = 0;
    fp->iob_CustomBuffer = NULL;
    fp->iob_isVBuffer = FALSE;

    /* Set up the new function pointer I/O path */
    fp->_read = __sread;
    fp->_write = __swrite;
    fp->_seek = __sseek;
    fp->_close = __sclose;
    fp->_seek64 = __sseek;
    fp->_cookie = fp;

    /* Link to the fd layer */
    fp->iob_Descriptor = file_descriptor;
    fp->iob_Lock = lock;
    fp->iob_TaskLock = NULL;

    /* Transition: also set up legacy fields */
    fp->iob_Action = __iob_hook_entry;
    fp->iob_SlotNumber = -1;  /* Not in the old __iob[] table */

    fp->_blksize = BUFSIZ;

    RETURN((FILE *) fp);
    return (FILE *) fp;
}
