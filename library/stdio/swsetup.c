/*
 * $Id: stdio_swsetup.c,v 1.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * Prepare a FILE stream for writing (newlib-inspired).
 * Called before any write operation to ensure the stream is in write mode.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/****************************************************************************/

/*
 * __swsetup — Prepare a stream for writing.
 *
 * Checks that writing is allowed, flushes any pending read data,
 * and ensures a buffer is allocated (lazy via __smakebuf).
 *
 * Returns 0 on success, EOF on error.
 */
int
__swsetup(struct _clib4 *__clib4, struct iob *fp) {
    ENTER();

    /* Check that we can write to this stream */
    if (cantwrite(__clib4, fp)) {
        __set_errno(EBADF);
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        RETURN(EOF);
        return EOF;
    }

    /* If there's pending read data, discard/flush it */
    if (fp->iob_BufferReadBytes > 0) {
        fp->iob_BufferReadBytes = 0;
        fp->iob_BufferPosition = 0;
    }

    /* Discard any ungetc buffer */
    if (HASUB(fp)) {
        FREEUB(__clib4, fp);
        CLEAR_FLAG(fp->iob_Flags, IOBF_UNGETC);
    }

    /* Clear EOF flag — writing resets it */
    CLEAR_FLAG(fp->iob_Flags, IOBF_EOF_REACHED);

    /* Ensure we have a buffer (lazy allocation) */
    if (fp->iob_Buffer == NULL) {
        __smakebuf(__clib4, fp);
    }

    /* Reset write position if not already writing */
    /* (num_write_bytes tracks how much has been written into the buffer) */

    RETURN(0);
    return 0;
}
