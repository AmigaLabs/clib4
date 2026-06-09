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
    unsigned long flags;
    int buffer_mode;

    ENTER();
    SHOWVALUE(c);
    SHOWPOINTER(stream);

    if (__builtin_expect(stream == NULL, 0)) {
        __set_errno_r(__clib4, EFAULT);
        RETURN(result);
        return result;
    }

    __flockfile_r(__clib4, stream);

    flags = fp->iob_Flags;

    /*
     * Ultra-fast path: stream is in-use + writable, buffer allocated,
     * no pending read data, and buffer has space. Inlined to eliminate
     * the __fputc_check() + __fputc() function calls (saves ~30-50
     * PPC cycles per call).
     */
    if (__builtin_expect(
            (flags & (IOBF_IN_USE | IOBF_WRITE)) == (IOBF_IN_USE | IOBF_WRITE) &&
            fp->iob_Buffer != NULL &&
            fp->iob_BufferReadBytes == 0, 1)) {

        if (__builtin_expect(fp->iob_BufferWriteBytes < fp->iob_BufferSize, 1)) {
            fp->iob_Buffer[fp->iob_BufferWriteBytes++] = c;
            buffer_mode = (flags & IOBF_BUFFER_MODE);
            /* Fully-buffered: nothing to do — most common case */
            if (__builtin_expect(buffer_mode == IOBF_BUFFER_MODE_FULL, 1)) {
                result = (c & 255);
                goto out;
            }
            /* Unbuffered or line-buffered with newline: flush */
            if (buffer_mode == IOBF_BUFFER_MODE_NONE ||
                (buffer_mode == IOBF_BUFFER_MODE_LINE && c == '\n')) {
                if (__sflush(__clib4, fp) != 0) {
                    fp->iob_BufferWriteBytes--;
                    goto out;  /* result is already EOF */
                }
            }
            result = (c & 255);
            goto out;
        }
        /* Buffer full — flush and store via __swbuf */
        result = __swbuf(__clib4, c, fp);
        goto out;
    }

    /* Slow path: first write, mode switch, or invalid stream */
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
    unsigned long flags;
    int buffer_mode;

    ENTER();
    SHOWVALUE(c);
    SHOWPOINTER(stream);

    if (__builtin_expect(stream == NULL, 0)) {
        __set_errno_r(__clib4, EFAULT);
        RETURN(result);
        return result;
    }

    __check_abort_f(__clib4);

    __flockfile_r(__clib4, stream);

    flags = fp->iob_Flags;

    /* Ultra-fast path — same as fputc() above */
    if (__builtin_expect(
            (flags & (IOBF_IN_USE | IOBF_WRITE)) == (IOBF_IN_USE | IOBF_WRITE) &&
            fp->iob_Buffer != NULL &&
            fp->iob_BufferReadBytes == 0, 1)) {

        if (__builtin_expect(fp->iob_BufferWriteBytes < fp->iob_BufferSize, 1)) {
            fp->iob_Buffer[fp->iob_BufferWriteBytes++] = c;
            buffer_mode = (flags & IOBF_BUFFER_MODE);
            if (__builtin_expect(buffer_mode == IOBF_BUFFER_MODE_FULL, 1)) {
                result = (c & 255);
                goto out;
            }
            if (buffer_mode == IOBF_BUFFER_MODE_NONE ||
                (buffer_mode == IOBF_BUFFER_MODE_LINE && c == '\n')) {
                if (__sflush(__clib4, fp) != 0) {
                    fp->iob_BufferWriteBytes--;
                    goto out;
                }
            }
            result = (c & 255);
            goto out;
        }
        result = __swbuf(__clib4, c, fp);
        goto out;
    }

    /* Slow path */
    if (__fputc_check(__clib4, stream) < 0)
        goto out;

    result = __fputc(__clib4, c, stream, (fp->iob_Flags & IOBF_BUFFER_MODE));

out:
    __funlockfile_r(__clib4, stream);

    RETURN(result);
    return result;
}