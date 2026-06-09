/*
 * $Id: stdio_ungetc.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * ungetc() — newlib-inspired rewrite with proper ungetc buffer (_ub) support.
 * Supports pushing back multiple characters (not just one).
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
ungetc(int c, FILE *stream) {
    return __ungetc_r(__CLIB4, c, stream);
}

int
__ungetc_r(struct _clib4 *__clib4, int c, FILE *stream) {
    struct iob *fp = (struct iob *) stream;
    int result = EOF;

    ENTER();

    if (stream == NULL) {
        __set_errno_r(__clib4, EFAULT);
        RETURN(result);
        return result;
    }

    /* Pushing back EOF is a no-op per C standard. */
    if (c == EOF) {
        RETURN(result);
        return result;
    }

    __flockfile_r(__clib4, stream);

    if (FLAG_IS_CLEAR(fp->iob_Flags, IOBF_IN_USE)) {
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    if (cantread(__clib4, fp)) {
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    /* Flush any pending write data before switching to read mode. */
    if (fp->iob_BufferWriteBytes > 0 && __sflush(__clib4, fp) != 0)
        goto out;

    /*
     * Simple case: if there's room in the buffer before the current
     * read position, just back up one byte. This is the common case
     * for a single ungetc() after a getc().
     */
    if (fp->iob_BufferPosition > 0 && !HASUB(fp)) {
        fp->iob_BufferPosition--;
        fp->iob_Buffer[fp->iob_BufferPosition] = (unsigned char) c;
        CLEAR_FLAG(fp->iob_Flags, IOBF_EOF_REACHED);
        result = (c & 0xFF);
        goto out;
    }

    /*
     * No room in the main buffer — use the ungetc pushback buffer (_ub).
     *
     * If the _ub is not active yet, start it with the small inline buffer
     * (_ubuf). If that's full, grow to a heap-allocated buffer.
     */
    if (!HASUB(fp)) {
        /* First pushback — initialize _ub with the inline buffer. */
        fp->_ub._base = fp->_ubuf;
        fp->_ub._size = 0;
        SET_FLAG(fp->iob_Flags, IOBF_UNGETC);
    }

    if (fp->_ub._size >= (int) sizeof(fp->_ubuf)) {
        /* Inline buffer is full — need a larger buffer. */
        int new_size = fp->_ub._size + 1;
        unsigned char *new_buf;

        if (fp->_ub._base == fp->_ubuf) {
            /* First growth: alloc and copy from inline. */
            new_buf = malloc(new_size);
            if (new_buf == NULL) {
                __set_errno_r(__clib4, ENOMEM);
                goto out;
            }
            memcpy(new_buf + 1, fp->_ubuf, fp->_ub._size);
        } else {
            /* Already on heap — realloc. */
            new_buf = realloc(fp->_ub._base, new_size);
            if (new_buf == NULL) {
                __set_errno_r(__clib4, ENOMEM);
                goto out;
            }
            /* shift existing data right by 1 to make room at front */
            memmove(new_buf + 1, new_buf, fp->_ub._size);
        }
        fp->_ub._base = new_buf;
    } else {
        /* Room in inline buffer — shift existing data right. */
        if (fp->_ub._size > 0)
            memmove(fp->_ub._base + 1, fp->_ub._base, fp->_ub._size);
    }

    fp->_ub._base[0] = (unsigned char) c;
    fp->_ub._size++;

    CLEAR_FLAG(fp->iob_Flags, IOBF_EOF_REACHED);
    result = (c & 0xFF);

out:
    __funlockfile_r(__clib4, stream);

    RETURN(result);
    return result;
}
