/*
 * $Id: stdio_ftello64.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * ftello64() — 64-bit version of ftell, newlib-inspired rewrite.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

_off64_t
ftello64(FILE *stream) {
    struct iob *fp = (struct iob *) stream;
    struct _clib4 *__clib4 = __CLIB4;
    _off64_t position;

    if (stream == NULL) {
        __set_errno(EFAULT);
        return (_off64_t) -1;
    }

    __flockfile_r(__clib4, stream);

    if (FLAG_IS_CLEAR(fp->iob_Flags, IOBF_IN_USE)) {
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno(EBADF);
        __funlockfile_r(__clib4, stream);
        return (_off64_t) -1;
    }

    /* Get underlying file position. */
    if (fp->_seek != NULL) {
        position = (_off64_t) fp->_seek(fp->_cookie, (fpos_t) 0, SEEK_CUR);
        if (position == (_off64_t) -1) {
            SET_FLAG(fp->iob_Flags, IOBF_ERROR);
            __funlockfile_r(__clib4, stream);
            return (_off64_t) -1;
        }
    } else if (fp->iob_Action != NULL) {
        struct file_action_message fam;
        fam.fam_Action = file_action_seek;
        fam.fam_Offset = 0;
        fam.fam_Mode = SEEK_CUR;
        position = (_off64_t)(*fp->iob_Action)(__clib4, fp, &fam);
        if (fam.fam_Error != OK) {
            SET_FLAG(fp->iob_Flags, IOBF_ERROR);
            __set_errno(fam.fam_Error);
            __funlockfile_r(__clib4, stream);
            return (_off64_t) -1;
        }
    } else {
        __set_errno(EBADF);
        __funlockfile_r(__clib4, stream);
        return (_off64_t) -1;
    }

    /* Adjust for unread buffered data. */
    if (READABLE_BYTES(fp) > 0)
        position -= READABLE_BYTES(fp);

    /* Adjust for ungetc pushback bytes. */
    if (HASUB(fp))
        position -= fp->_ub._size;

    /* Adjust for unwritten buffered data. */
    if (fp->iob_BufferWriteBytes > 0)
        position += fp->iob_BufferWriteBytes;

    __funlockfile_r(__clib4, stream);
    return position;
}