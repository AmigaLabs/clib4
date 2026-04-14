/*
 * $Id: stdio_setvbuf.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * setvbuf() — newlib-inspired rewrite aware of lazy buffer allocation.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
setvbuf(FILE *stream, char *buf, int bufmode, size_t size) {
    struct iob *fp = (struct iob *) stream;
    char *new_buffer = NULL;
    int result = EOF;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    if (stream == NULL) {
        __set_errno_r(__clib4, EFAULT);
        RETURN(result);
        return result;
    }

    if (bufmode < IOBF_BUFFER_MODE_FULL || bufmode > IOBF_BUFFER_MODE_NONE) {
        __set_errno_r(__clib4, EINVAL);
        RETURN(result);
        return result;
    }

    if ((int) size < 0) {
        __set_errno_r(__clib4, EINVAL);
        RETURN(result);
        return result;
    }

    __flockfile_r(__clib4, stream);

    if (FLAG_IS_CLEAR(fp->iob_Flags, IOBF_IN_USE)) {
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    /* Default buffer size if 0 is specified. */
    if (size == 0) {
        size = BUFSIZ;
        buf = NULL;
    }

    /* Allocate a new buffer if needed (not unbuffered, no user buffer). */
    if (bufmode != IOBF_BUFFER_MODE_NONE && size > 0 && buf == NULL) {
        new_buffer = AllocVecTags(size,
            AVT_Type, MEMF_SHARED,
            AVT_ClearWithValue, 0,
            AVT_Alignment, __clib4->__cache_line_size,
            TAG_DONE);
        if (new_buffer == NULL) {
            __set_errno_r(__clib4, ENOBUFS);
            goto out;
        }
    }

    /* Flush and discard any buffered data. */
    if (fp->iob_BufferWriteBytes > 0)
        __sflush(__clib4, fp);

    fp->iob_BufferReadBytes = 0;
    fp->iob_BufferPosition = 0;
    fp->iob_BufferWriteBytes = 0;

    /* Free any previously allocated buffer. */
    if (fp->iob_CustomBuffer != NULL) {
        if (fp->iob_isVBuffer) {
            FreeVec(fp->iob_CustomBuffer);
            fp->iob_isVBuffer = FALSE;
        } else {
            free(fp->iob_CustomBuffer);
        }
        fp->iob_CustomBuffer = NULL;
    }

    /* Also free IOBF_MALLOC_BUF buffer if applicable. */
    if (FLAG_IS_SET(fp->iob_Flags, IOBF_MALLOC_BUF) && fp->iob_Buffer != NULL) {
        FreeVec(fp->iob_Buffer);
        fp->iob_Buffer = NULL;
        CLEAR_FLAG(fp->iob_Flags, IOBF_MALLOC_BUF);
    }

    if (bufmode == IOBF_BUFFER_MODE_NONE) {
        /* Unbuffered: use the single-byte fallback. */
        fp->iob_Buffer = &fp->iob_SingleByte;
        fp->iob_BufferSize = 1;
    } else {
        if (buf != NULL) {
            /* User-supplied buffer. */
            fp->iob_Buffer = (UBYTE *) buf;
        } else {
            /* Our newly allocated buffer. */
            fp->iob_Buffer = (UBYTE *) new_buffer;
            fp->iob_isVBuffer = TRUE;
            fp->iob_CustomBuffer = new_buffer;
            new_buffer = NULL;  /* prevent cleanup below */
        }
        fp->iob_BufferSize = size;
    }

    fp->_blksize = fp->iob_BufferSize;
    fp->iob_Flags = (fp->iob_Flags & ~IOBF_BUFFER_MODE) | bufmode;

    result = OK;

out:
    __funlockfile_r(__clib4, stream);

    if (new_buffer != NULL)
        FreeVec(new_buffer);

    RETURN(result);
    return result;
}
