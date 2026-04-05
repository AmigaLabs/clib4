/*
 * $Id: stdio_flushiobwritebuffer.c,v 1.10 2025-01-01 09:02:51 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/* This would otherwise generate far too much (useless) debug output. */
#ifdef DEBUG
#undef DEBUG
#endif /* DEBUG */

/*
 * __sflush — Unified stream flush (newlib-style).
 *
 * Writes any pending buffered data to the underlying fd via
 * the fp->_write function pointer. Resets num_write_bytes to 0.
 *
 * Returns 0 on success, EOF on error.
 */
int
__sflush(struct _clib4 *__clib4, struct iob *fp) {
    ssize_t n;
    ssize_t written;
    unsigned char *p;

    assert(fp != NULL);
    assert(FLAG_IS_SET(fp->iob_Flags, IOBF_IN_USE));

    if (fp->iob_BufferWriteBytes <= 0) {
        return 0;
    }

    /* Use the new function pointer path if available */
    if (fp->_write != NULL) {
        p = fp->iob_Buffer;
        n = fp->iob_BufferWriteBytes;

        while (n > 0) {
            written = fp->_write(fp->_cookie, (const char *) p, (int) n);
            if (written <= 0) {
                SET_FLAG(fp->iob_Flags, IOBF_ERROR);
                return EOF;
            }
            p += written;
            n -= written;
        }

        fp->iob_BufferWriteBytes = 0;
        return 0;
    }

    /* Fallback: use legacy iob_Action path (transition) */
    if (fp->iob_Action != NULL) {
        struct file_action_message fam;

        fam.fam_Action = file_action_write;
        fam.fam_Data = (char *) fp->iob_Buffer;
        fam.fam_Size = fp->iob_BufferWriteBytes;

        if ((*fp->iob_Action)(__clib4, fp, &fam) == EOF) {
            SET_FLAG(fp->iob_Flags, IOBF_ERROR);
            __set_errno_r(__clib4, fam.fam_Error);
            return EOF;
        }

        fp->iob_BufferWriteBytes = 0;
        return 0;
    }

    /* Neither path available */
    SET_FLAG(fp->iob_Flags, IOBF_ERROR);
    __set_errno(EBADF);
    return EOF;
}


