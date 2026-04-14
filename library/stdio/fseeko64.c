/*
 * $Id: stdio_fseeko64.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * fseeko64() — 64-bit seek, newlib-inspired rewrite using function pointers.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

_off64_t
fseeko64(FILE *stream, _off64_t offset, int wherefrom) {
    struct iob *fp = (struct iob *) stream;
    struct _clib4 *__clib4 = __CLIB4;

    if (stream == NULL) {
        __set_errno_r(__clib4, EFAULT);
        return (_off64_t) -1;
    }

    if (wherefrom < SEEK_SET || wherefrom > SEEK_END) {
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno_r(__clib4, EINVAL);
        return (_off64_t) -1;
    }

    __check_abort_f(__clib4);
    __flockfile_r(__clib4, stream);

    if (FLAG_IS_CLEAR(fp->iob_Flags, IOBF_IN_USE)) {
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno_r(__clib4, EBADF);
        __funlockfile_r(__clib4, stream);
        return (_off64_t) -1;
    }

    /* Discard ungetc pushback buffer. */
    if (HASUB(fp)) {
        FREEUB(__clib4, fp);
        CLEAR_FLAG(fp->iob_Flags, IOBF_UNGETC);
    }

    /* Clear EOF condition. */
    CLEAR_FLAG(fp->iob_Flags, IOBF_EOF_REACHED);

    /* Try to convert SEEK_SET into a buffer-relative SEEK_CUR. */
    if (wherefrom == SEEK_SET && READABLE_BYTES(fp) > 0) {
        _off64_t curpos = ftello64(stream);
        if (curpos >= 0) {
            offset -= curpos;
            wherefrom = SEEK_CUR;
        }
    }

    /* Try buffer-relative adjustment for SEEK_CUR. */
    if (wherefrom == SEEK_CUR && READABLE_BYTES(fp) > 0) {
        int64_t unread = (int64_t) READABLE_BYTES(fp);
        if ((offset >= 0 && offset <= unread) ||
            (offset < 0 && (-offset) <= (int64_t) fp->iob_BufferPosition)) {
            fp->iob_BufferPosition += (int) offset;
            __funlockfile_r(__clib4, stream);
            return 0;
        }
        /* Adjust offset to account for unread buffered data. */
        offset -= unread;
    }

    /* Flush write buffer if dirty. */
    if (fp->iob_BufferWriteBytes > 0) {
        if (__sflush(__clib4, fp) != 0) {
            __funlockfile_r(__clib4, stream);
            return (_off64_t) -1;
        }
    }

    /* Drop the read buffer. */
    fp->iob_BufferPosition = 0;
    fp->iob_BufferReadBytes = 0;

    /* Perform the actual seek. */
    if (fp->_seek != NULL) {
        fpos_t pos = fp->_seek(fp->_cookie, (fpos_t) offset, wherefrom);
        if (pos == (fpos_t) -1) {
            SET_FLAG(fp->iob_Flags, IOBF_ERROR);
            __funlockfile_r(__clib4, stream);
            return (_off64_t) -1;
        }
        /* Update cached offset. */
        fp->_offset = pos;
        SET_FLAG(fp->iob_Flags, IOBF_OFF);
    } else if (fp->iob_Action != NULL) {
        struct file_action_message fam;
        fam.fam_Action = file_action_seek;
        fam.fam_Offset = offset;
        fam.fam_Mode = wherefrom;
        _off64_t pos = (_off64_t)(*fp->iob_Action)(__clib4, fp, &fam);
        if (fam.fam_Error != OK) {
            SET_FLAG(fp->iob_Flags, IOBF_ERROR);
            __set_errno_r(__clib4, fam.fam_Error);
            __funlockfile_r(__clib4, stream);
            return (_off64_t) -1;
        }
        (void) pos;
    } else {
        __set_errno_r(__clib4, EBADF);
        __funlockfile_r(__clib4, stream);
        return (_off64_t) -1;
    }

    __funlockfile_r(__clib4, stream);
    return 0;
}
