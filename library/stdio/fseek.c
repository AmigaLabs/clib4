/*
 * $Id: stdio_fseek.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * fseek() — newlib-inspired rewrite using function pointers.
 * Flushes write buffer, discards ungetc/read buffers, optimizes
 * seeks within the read buffer when possible.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
fseek(FILE *stream, long int offset, int wherefrom) {
    struct iob *fp = (struct iob *) stream;
    struct _clib4 *__clib4 = __CLIB4;
    int result = EOF;
    fpos_t pos;

    ENTER();

    if (stream == NULL) {
        __set_errno(EFAULT);
        RETURN(result);
        return result;
    }

    if (FLAG_IS_CLEAR(fp->iob_Flags, IOBF_IN_USE)) {
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno(EBADF);
        RETURN(result);
        return result;
    }

    if (wherefrom < SEEK_SET || wherefrom > SEEK_END) {
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno(EINVAL);
        RETURN(result);
        return result;
    }

    /* Flush any pending write data. */
    if (fp->iob_BufferWriteBytes > 0) {
        if (__sflush(__clib4, fp) != 0)
            goto out;
    }

    /* Discard any ungetc pushback buffer. */
    if (HASUB(fp)) {
        FREEUB(__clib4, fp);
        CLEAR_FLAG(fp->iob_Flags, IOBF_UNGETC);
    }

    /* Clear EOF condition. */
    CLEAR_FLAG(fp->iob_Flags, IOBF_EOF_REACHED);

    /*
     * Optimization: for SEEK_CUR with data in the read buffer,
     * try to adjust the buffer position without a real seek syscall.
     *
     * For SEEK_SET, convert to SEEK_CUR relative to current position.
     */
    if (wherefrom == SEEK_SET && READABLE_BYTES(fp) > 0) {
        long int current = ftell(stream);
        if (current >= 0) {
            offset -= current;
            wherefrom = SEEK_CUR;
        }
    }

    if (wherefrom == SEEK_CUR && READABLE_BYTES(fp) > 0) {
        long int unread = READABLE_BYTES(fp);
        if (offset >= -((long int) fp->iob_BufferPosition) && offset < unread) {
            fp->iob_BufferPosition += offset;
            RETURN(OK);
            return OK;
        }
        /* Adjust offset to account for unread buffered data. */
        offset -= unread;
    }

    /* Discard the read buffer. */
    fp->iob_BufferReadBytes = 0;
    fp->iob_BufferPosition = 0;

    /* Perform the actual seek via function pointer. */
    if (fp->_seek != NULL) {
        pos = fp->_seek(fp->_cookie, (fpos_t) offset, wherefrom);
        if (pos == (fpos_t) -1) {
            SET_FLAG(fp->iob_Flags, IOBF_ERROR);
            goto out;
        }
        /* Update cached offset. */
        fp->_offset = pos;
        SET_FLAG(fp->iob_Flags, IOBF_OFF);
    } else if (fp->iob_Action != NULL) {
        /* Legacy fallback via iob_Action. */
        struct file_action_message fam;
        fam.fam_Action = file_action_seek;
        fam.fam_Offset = offset;
        fam.fam_Mode = wherefrom;
        pos = (fpos_t)(*fp->iob_Action)(__clib4, fp, &fam);
        if (fam.fam_Error != OK) {
            SET_FLAG(fp->iob_Flags, IOBF_ERROR);
            __set_errno(fam.fam_Error);
            goto out;
        }
    } else {
        __set_errno(EBADF);
        goto out;
    }

    result = OK;

out:
    RETURN(result);
    return result;
}
