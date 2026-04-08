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
    if (__builtin_expect(fp->iob_BufferReadBytes > 0 &&
        fp->iob_BufferPosition < fp->iob_BufferReadBytes, 1)) {
        RETURN(0);
        return 0;
    }

    /*
     * BSD lflush: before reading from a line-buffered stream, flush all
     * line-buffered output streams. This ensures prompts written to stdout
     * are visible before we block waiting for input on stdin.
     * This matches the old __fill_iob_read_buffer() behavior.
     */
    if ((fp->iob_Flags & IOBF_BUFFER_MODE) == IOBF_BUFFER_MODE_LINE) {
        if (__flush_all_files(__clib4, IOBF_BUFFER_MODE_LINE) < 0) {
            RETURN(EOF);
            return EOF;
        }
    }

    /* If there is pending write data, flush it first */
    if (__builtin_expect(fp->iob_BufferWriteBytes > 0, 0)) {
        if (__sflush(__clib4, fp) != 0) {
            RETURN(EOF);
            return EOF;
        }
    }

    /* Discard any active ungetc buffer */
    if (__builtin_expect(HASUB(fp), 0)) {
        FREEUB(__clib4, fp);
        CLEAR_FLAG(fp->iob_Flags, IOBF_UNGETC);
    }

    /* Ensure we have a buffer (lazy allocation) */
    if (__builtin_expect(fp->iob_Buffer == NULL, 0)) {
        __smakebuf(__clib4, fp);
    }

    /* Reset buffer position before filling */
    fp->iob_BufferPosition = 0;
    fp->iob_BufferReadBytes = 0;

    /* Call the read function to fill the buffer */
    if (__builtin_expect(fp->_read != NULL, 1)) {
        n = fp->_read(fp->_cookie, (char *) fp->iob_Buffer, (int) fp->iob_BufferSize);
    } else if (fp->iob_Action != NULL) {
        /* Fallback: use legacy iob_Action path (for string streams like sscanf) */
        struct file_action_message fam;
        fam.fam_Action = file_action_read;
        fam.fam_Data = (char *) fp->iob_Buffer;
        fam.fam_Size = fp->iob_BufferSize;
        fam.fam_Error = 0;
        n = (ssize_t)(*fp->iob_Action)(__clib4, fp, &fam);
        if (n == EOF && fam.fam_Error != OK) {
            SET_FLAG(fp->iob_Flags, IOBF_ERROR);
            __set_errno(fam.fam_Error);
            RETURN(EOF);
            return EOF;
        }
    } else {
        __set_errno(EBADF);
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        RETURN(EOF);
        return EOF;
    }

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
