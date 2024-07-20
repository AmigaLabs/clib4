/*
 * $Id: stdio_ftello64.c,v 1.0 2021-02-05 13:30:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

_off64_t
ftello64(FILE *stream) {
    struct iob *file = (struct iob *) stream;
    struct file_action_message fam;
    int64_t result = ERROR;
    int64_t position;
    struct _clib4 *__clib4 = __CLIB4;

    assert(stream != NULL);

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");
        __set_errno(EFAULT);

        RETURN(result);
        return (result);
    }

    assert(__is_valid_iob(__clib4, file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE)) {
        SHOWMSG("this file is not even in use");
        SET_FLAG(file->iob_Flags, IOBF_ERROR);
        __set_errno(EBADF);

        RETURN(result);
        return (result);
    }

    __flockfile_r(__clib4, stream);

    SHOWMSG("calling the hook");
    SHOWPOINTER(&fam);

    fam.fam_Action = file_action_seek;
    fam.fam_Offset = 0;
    fam.fam_Mode = SEEK_CUR;

    SHOWVALUE(fam.fam_Offset);
    SHOWVALUE(fam.fam_Mode);

    assert(file->iob_Action != NULL);

    position = (*file->iob_Action)(__clib4, file, &fam);
    if (fam.fam_Error != OK) {
        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno(fam.fam_Error);

        goto out;
    }

    if (__iob_read_buffer_is_valid(file)) {
        /* Subtract the number of bytes still in the buffer which have
		 * not been read before.
		 */
        position -= __iob_num_unread_bytes(file);
    } else if (__iob_write_buffer_is_valid(file)) {
        /* Add the number of bytes still stored in the buffer which have
		 * not been written to disk yet.
		 */
        position += __iob_num_unwritten_bytes(file);
    }

    result = position;

out:

    __funlockfile_r(__clib4, stream);

    return (_off64_t) result;
}