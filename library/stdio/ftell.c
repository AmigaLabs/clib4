/*
 * $Id: stdio_ftell.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * ftell() — newlib-inspired rewrite using function pointers.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

long int
ftell(FILE *stream) {
    struct iob *fp = (struct iob *) stream;
    struct _clib4 *__clib4 = __CLIB4;
    fpos_t position;

    if (stream == NULL) {
        __set_errno_r(__clib4, EFAULT);
        return -1L;
    }

    __check_abort_f(__clib4);
    __flockfile_r(__clib4, stream);

    if (FLAG_IS_CLEAR(fp->iob_Flags, IOBF_IN_USE)) {
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno_r(__clib4, EBADF);
        __funlockfile_r(__clib4, stream);
        return -1L;
    }

    /* Get the underlying file position via seek(0, SEEK_CUR). */
    if (fp->_seek != NULL) {
        position = fp->_seek(fp->_cookie, (fpos_t) 0, SEEK_CUR);
        if (position == (fpos_t) -1) {
            SET_FLAG(fp->iob_Flags, IOBF_ERROR);
            __funlockfile_r(__clib4, stream);
            return -1L;
        }
    } else if (fp->iob_Action != NULL) {
        /* Legacy fallback. */
        struct file_action_message fam;
        fam.fam_Action = file_action_seek;
        fam.fam_Offset = 0;
        fam.fam_Mode = SEEK_CUR;
        position = (fpos_t)(*fp->iob_Action)(__clib4, fp, &fam);
        if (fam.fam_Error != OK) {
            SET_FLAG(fp->iob_Flags, IOBF_ERROR);
            __set_errno_r(__clib4, fam.fam_Error);
            __funlockfile_r(__clib4, stream);
            return -1L;
        }
    } else {
        __set_errno_r(__clib4, EBADF);
        __funlockfile_r(__clib4, stream);
        return -1L;
    }

    /* Adjust for unread buffered data. */
    if (READABLE_BYTES(fp) > 0) {
        position -= READABLE_BYTES(fp);
    }

    /* Adjust for ungetc pushback bytes. */
    if (HASUB(fp))
        position -= fp->_ub._size;

    /* Adjust for unwritten buffered data. */
    if (fp->iob_BufferWriteBytes > 0) {
        position += fp->iob_BufferWriteBytes;
    }

    __funlockfile_r(__clib4, stream);
    return (long int) position;
}
