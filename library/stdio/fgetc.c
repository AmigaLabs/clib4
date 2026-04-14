/*
 * $Id: stdio_fgetc.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * fgetc() — newlib-inspired rewrite using __srefill() for buffer management.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/*
 * __fgetc — internal unlocked getc, called by the __getc() macro
 * when the buffer is empty. Checks ungetc buffer first, then
 * refills via __srefill() and returns the next byte, or EOF.
 */
int
__fgetc(struct _clib4 *__clib4, FILE *stream) {
    struct iob *fp = (struct iob *) stream;

    assert(stream != NULL);
    assert(FLAG_IS_SET(fp->iob_Flags, IOBF_IN_USE));

    /* Check the ungetc pushback buffer first. */
    if (HASUB(fp) && fp->_ub._size > 0) {
        unsigned char c = fp->_ub._base[0];
        fp->_ub._size--;
        if (fp->_ub._size > 0) {
            memmove(fp->_ub._base, fp->_ub._base + 1, fp->_ub._size);
        } else {
            FREEUB(__clib4, fp);
            CLEAR_FLAG(fp->iob_Flags, IOBF_UNGETC);
        }
        return (int) c;
    }

    if (__srefill(__clib4, fp) != 0)
        return EOF;

    return fp->iob_Buffer[fp->iob_BufferPosition++];
}

/*
 * __fgetc_check — validate that a stream is readable and flush
 * any pending write data. Called before a read sequence.
 */
int
__fgetc_check(struct _clib4 *__clib4, FILE *stream) {
    struct iob *fp = (struct iob *) stream;

    if (stream == NULL) {
        __set_errno_r(__clib4, EFAULT);
        return EOF;
    }

    if (FLAG_IS_CLEAR(fp->iob_Flags, IOBF_IN_USE)) {
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno_r(__clib4, EBADF);
        return EOF;
    }

    if (cantread(__clib4, fp)) {
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno_r(__clib4, EBADF);
        return EOF;
    }

    /* Flush any pending write data before switching to read mode. */
    if (fp->iob_BufferWriteBytes > 0 && __sflush(__clib4, fp) != 0)
        return EOF;

    return OK;
}

int
fgetc(FILE *stream) {
    int result = EOF;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();
    SHOWPOINTER(stream);

    if (stream == NULL) {
        __set_errno_r(__clib4, EFAULT);
        RETURN(result);
        return result;
    }

    __check_abort_f(__clib4);

    __flockfile_r(__clib4, stream);

    if (__fgetc_check(__clib4, stream) < 0)
        goto out;

    result = __getc(__clib4, stream);

out:
    __funlockfile_r(__clib4, stream);

    RETURN(result);
    return result;
}
