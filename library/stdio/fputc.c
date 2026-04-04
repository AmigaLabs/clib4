/*
 * $Id: stdio_fputc.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * fputc() — newlib-inspired rewrite using __swsetup()/__sflush()/__swbuf().
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/*
 * __fputc_check — validate that a stream is writable and prepare
 * it for writing (flush read data, allocate buffer). Called once
 * before a write sequence.
 */
int
__fputc_check(struct _clib4 *__clib4, FILE *stream) {
    struct iob *fp = (struct iob *) stream;

    if (stream == NULL) {
        __set_errno(EBADF);
        return EOF;
    }

    if (FLAG_IS_CLEAR(fp->iob_Flags, IOBF_IN_USE)) {
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno_r(__clib4, EBADF);
        return EOF;
    }

    if (cantwrite(__clib4, fp)) {
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno_r(__clib4, EBADF);
        return EOF;
    }

    /*
     * Skip __swsetup if buffer is already allocated, no pending read
     * data, and no ungetc buffer — avoids a function call per write.
     */
    if (fp->iob_Buffer != NULL && fp->iob_BufferReadBytes == 0 && !HASUB(fp)) {
        return OK;
    }

    /* Slow path: first write, or mode switch needed. */
    if (__swsetup(__clib4, fp) != 0)
        return EOF;

    return OK;
}

/*
 * __fputc — internal unlocked putc.
 * Stores byte in write buffer; flushes if needed based on buffer mode.
 */
int
__fputc(struct _clib4 *__clib4, int c, FILE *stream, int buffer_mode) {
    struct iob *fp = (struct iob *) stream;

    assert(stream != NULL);
    assert(FLAG_IS_SET(fp->iob_Flags, IOBF_IN_USE));

    /* If buffer is full, flush via __swbuf which also stores the byte. */
    if (WRITABLE_SPACE(fp) == 0)
        return __swbuf(__clib4, c, fp);

    fp->iob_Buffer[fp->iob_BufferWriteBytes++] = c;

    /* For unbuffered or line-buffered + newline, flush immediately. */
    if (buffer_mode == IOBF_BUFFER_MODE_NONE || (buffer_mode == IOBF_BUFFER_MODE_LINE && c == '\n')) {
        if (__sflush(__clib4, fp) != 0) {
            fp->iob_BufferWriteBytes--;
            return EOF;
        }
    }

    return (c & 255);
}

int
fputc(int c, FILE *stream) {
    struct iob *fp = (struct iob *) stream;
    int result = EOF;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();
    SHOWVALUE(c);
    SHOWPOINTER(stream);

    if (stream == NULL) {
        __set_errno_r(__clib4, EFAULT);
        RETURN(result);
        return result;
    }

    __flockfile_r(__clib4, stream);

    if (__fputc_check(__clib4, stream) < 0)
        goto out;

    result = __fputc(__clib4, c, stream, (fp->iob_Flags & IOBF_BUFFER_MODE));

out:
    __funlockfile_r(__clib4, stream);

    RETURN(result);
    return result;
}

int
__fputc_r(struct _clib4 *__clib4, int c, FILE *stream) {
    struct iob *fp = (struct iob *) stream;
    int result = EOF;

    ENTER();
    SHOWVALUE(c);
    SHOWPOINTER(stream);

    if (stream == NULL) {
        __set_errno_r(__clib4, EFAULT);
        RETURN(result);
        return result;
    }

    __check_abort_f(__clib4);

    __flockfile_r(__clib4, stream);

    if (__fputc_check(__clib4, stream) < 0)
        goto out;

    result = __fputc(__clib4, c, stream, (fp->iob_Flags & IOBF_BUFFER_MODE));

out:
    __funlockfile_r(__clib4, stream);

    RETURN(result);
    return result;
}