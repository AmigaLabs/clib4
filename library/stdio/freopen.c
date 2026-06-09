/*
 * $Id: stdio_freopen.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * freopen() — newlib-inspired rewrite.
 * Reuses the same FILE slot: flushes/closes the old fd, opens new fd,
 * reinitializes the iob.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

FILE *
freopen(const char *filename, const char *mode, FILE *stream) {
    struct iob *fp = (struct iob *) stream;
    struct _clib4 *__clib4 = __CLIB4;
    int open_mode;
    int file_descriptor;
    ULONG file_flags = 0;
    int saved_flags;

    if (mode == NULL || stream == NULL) {
        __set_errno_r(__clib4, EFAULT);
        return NULL;
    }

    __check_abort_f(__clib4);
    __flockfile_r(__clib4, stream);

    /* Flush any buffered write data. */
    if (fp->iob_BufferWriteBytes > 0)
        (void) __sflush(__clib4, fp);

    /* Discard ungetc pushback buffer. */
    if (HASUB(fp)) {
        FREEUB(__clib4, fp);
        CLEAR_FLAG(fp->iob_Flags, IOBF_UNGETC);
    }

    /* Close the old underlying file descriptor. */
    if (fp->_close != NULL)
        (*fp->_close)(fp->_cookie);
    else if (fp->iob_Descriptor >= 0)
        close(fp->iob_Descriptor);

    /* Free the buffer if we allocated it. */
    if (FLAG_IS_SET(fp->iob_Flags, IOBF_MALLOC_BUF) && fp->iob_Buffer != NULL) {
        FreeVec(fp->iob_Buffer);
        fp->iob_Buffer = NULL;
        CLEAR_FLAG(fp->iob_Flags, IOBF_MALLOC_BUF);
    }
    if (fp->iob_CustomBuffer != NULL) {
        if (fp->iob_isVBuffer)
            FreeVec(fp->iob_CustomBuffer);
        else
            free(fp->iob_CustomBuffer);
        fp->iob_CustomBuffer = NULL;
    }

    /* Parse the mode string. */
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
            goto fail;
    }

    if ((mode[1] == '+') || (mode[1] != '\0' && mode[2] == '+')) {
        CLEAR_FLAG(open_mode, O_RDONLY);
        CLEAR_FLAG(open_mode, O_WRONLY);
        SET_FLAG(open_mode, O_RDWR);
    } else if (mode[1] != '\0' && mode[1] == 'b' && mode[2] == 'l') {
        SET_FLAG(open_mode, O_LITTLE_ENDIAN);
        SET_FLAG(file_flags, IOBF_LITTLE_ENDIAN);
    }

    /* If filename is NULL, try to change mode of existing fd (C11 7.21.5.4).
       We don't support that on AmigaOS, so just return NULL. */
    if (filename == NULL)
        goto fail;

    /* Open the new file. */
    file_descriptor = __open_r(__clib4, filename, open_mode);
    if (file_descriptor < 0)
        goto fail;

    /* Reinitialize the iob for the new file. */
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
    fp->iob_Action = __iob_hook_entry;

    /* Reset newlib-style internal state. */
    fp->_ub._base = NULL;
    fp->_ub._size = 0;
    fp->_lb._base = NULL;
    fp->_lb._size = 0;
    fp->_offset = 0;
    fp->_blksize = BUFSIZ;
    CLEAR_FLAG(fp->iob_Flags, IOBF_OFF);

    __funlockfile_r(__clib4, stream);
    return (FILE *) fp;

fail:
    /* Mark the stream as no longer in use. */
    CLEAR_FLAG(fp->iob_Flags, IOBF_IN_USE);
    fp->iob_Descriptor = -1;
    __funlockfile_r(__clib4, stream);
    return NULL;
}
