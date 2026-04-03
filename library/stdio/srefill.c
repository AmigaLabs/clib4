/*
 * $Id: stdio_srefill.c,v 1.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * Refill a FILE's read buffer (newlib-inspired).
 * Replaces the old __fill_iob_read_buffer() mechanism.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/****************************************************************************/

/*
 * __srefill — Refill the read buffer of a FILE stream.
 *
 * If there is unread data still in the buffer, this is a no-op
 * (returns 0). Otherwise:
 *   1. If write data is pending, flush it first via __sflush()
 *   2. Call fp->_read(fp->_cookie, ...) to fetch new data
 *   3. Update buffer position and num_read_bytes
 *   4. Set IOBF_EOF_REACHED if we got 0 bytes (EOF)
 *
 * Returns 0 on success, EOF on error or end-of-file.
 */
int
__srefill(struct _clib4 *__clib4, struct iob *fp) {
    ssize_t n;

    ENTER();

    /* If there's still unread data in the buffer, nothing to do */
    if (fp->iob_BufferReadBytes > 0 &&
        fp->iob_BufferPosition < fp->iob_BufferReadBytes) {
        RETURN(0);
        return 0;
    }

    /* Check that we can actually read from this stream */
    if (cantread(__clib4, fp)) {
        __set_errno(EBADF);
        RETURN(EOF);
        return EOF;
    }

    /* If there is pending write data, flush it first */
    if (fp->iob_BufferWriteBytes > 0) {
        if (__sflush(__clib4, fp) != 0) {
            RETURN(EOF);
            return EOF;
        }
    }

    /* Discard any active ungetc buffer */
    if (HASUB(fp)) {
        /* Restore the real buffer state from before ungetc */
        FREEUB(__clib4, fp);
        CLEAR_FLAG(fp->iob_Flags, IOBF_UNGETC);
    }

    /* Ensure we have a buffer (lazy allocation) */
    if (fp->iob_Buffer == NULL) {
        __smakebuf(__clib4, fp);
    }

    /* Reset buffer position before filling */
    fp->iob_BufferPosition = 0;
    fp->iob_BufferReadBytes = 0;

    /* Call the read function to fill the buffer */
    if (fp->_read == NULL) {
        __set_errno(EBADF);
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        RETURN(EOF);
        return EOF;
    }

    n = fp->_read(fp->_cookie, (char *) fp->iob_Buffer, (int) fp->iob_BufferSize);

    if (n <= 0) {
        if (n == 0) {
            SET_FLAG(fp->iob_Flags, IOBF_EOF_REACHED);
        } else {
            SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        }
        RETURN(EOF);
        return EOF;
    }

    fp->iob_BufferReadBytes = n;
    fp->iob_BufferPosition = 0;

    RETURN(0);
    return 0;
}
