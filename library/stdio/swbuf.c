/*
 * $Id: stdio_swbuf.c,v 1.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * Handle write buffer overflow for a single byte (newlib-inspired).
 * Called by __putc_unlocked() when the write buffer is full.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/****************************************************************************/

/*
 * __swbuf — Write a single byte to a stream when the buffer is full.
 *
 * This is the slow path for putc() / __putc_unlocked().
 * It flushes the existing buffer, then stores the byte.
 *
 * For line-buffered streams, also flushes if the byte is '\n'.
 *
 * Returns the byte written (as unsigned char cast to int), or EOF on error.
 */
int
__swbuf(struct _clib4 *__clib4, int c, struct iob *fp) {
    unsigned char uc;

    ENTER();

    /* Prepare the stream for writing if needed */
    if (__swsetup(__clib4, fp) != 0) {
        RETURN(EOF);
        return EOF;
    }

    /* Ensure we have a buffer (lazy allocation) */
    if (fp->iob_Buffer == NULL) {
        __smakebuf(__clib4, fp);
    }

    uc = (unsigned char) c;

    /*
     * For unbuffered streams, write the byte directly.
     */
    if ((fp->iob_Flags & IOBF_BUFFER_MODE) == IOBF_BUFFER_MODE_NONE) {
        if (fp->_write != NULL) {
            if (fp->_write(fp->_cookie, (const char *) &uc, 1) != 1) {
                SET_FLAG(fp->iob_Flags, IOBF_ERROR);
                RETURN(EOF);
                return EOF;
            }
        } else if (fp->iob_Action != NULL) {
            /* Fallback: legacy iob_Action path (string streams) */
            struct file_action_message fam;
            fam.fam_Action = file_action_write;
            fam.fam_Data = (char *) &uc;
            fam.fam_Size = 1;
            if ((*fp->iob_Action)(__clib4, fp, &fam) == EOF) {
                SET_FLAG(fp->iob_Flags, IOBF_ERROR);
                RETURN(EOF);
                return EOF;
            }
        } else {
            __set_errno(EBADF);
            SET_FLAG(fp->iob_Flags, IOBF_ERROR);
            RETURN(EOF);
            return EOF;
        }

        RETURN((int) uc);
        return (int) uc;
    }

    /*
     * If the buffer is full, flush it first.
     */
    if (fp->iob_BufferWriteBytes >= fp->iob_BufferSize) {
        if (__sflush(__clib4, fp) != 0) {
            SET_FLAG(fp->iob_Flags, IOBF_ERROR);
            RETURN(EOF);
            return EOF;
        }
    }

    /* Store the byte in the buffer */
    fp->iob_Buffer[fp->iob_BufferWriteBytes++] = uc;

    /*
     * For line-buffered streams, flush on newline.
     */
    if ((fp->iob_Flags & IOBF_BUFFER_MODE) == IOBF_BUFFER_MODE_LINE && uc == '\n') {
        if (__sflush(__clib4, fp) != 0) {
            SET_FLAG(fp->iob_Flags, IOBF_ERROR);
            RETURN(EOF);
            return EOF;
        }
    }

    RETURN((int) uc);
    return (int) uc;
}
