/*
 * $Id: stdio_fgetc.c,v 1.8 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
__fgetc(struct _clib4 *__clib4, FILE *stream) {
    struct iob *file = (struct iob *) stream;
    int result = EOF;

    ENTER();
    SHOWPOINTER(stream);

    assert(stream != NULL);

    assert(__is_valid_iob(__clib4, file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    if (__iob_read_buffer_is_empty(file)) {
        if (__fill_iob_read_buffer(__clib4, file) < 0)
            goto out;

        if (__iob_read_buffer_is_empty(file)) {
            SET_FLAG(file->iob_Flags, IOBF_EOF_REACHED);

            goto out;
        }
    }

    result = file->iob_Buffer[file->iob_BufferPosition++];

out:

    LEAVE();
    return (result);
}

int
__fgetc_check(struct _clib4 *__clib4, FILE *stream) {
    struct iob *file = (struct iob *) stream;
    int result = EOF;

    assert(stream != NULL);

    if (stream == NULL) {
        SHOWMSG("invalid parameters");
        __set_errno_r(__clib4, EFAULT);

        goto out;
    }

    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE)) {
        SHOWMSG("this file is not even in use");

        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_READ)) {
        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    if (__iob_write_buffer_is_valid(file) && __flush_iob_write_buffer(__clib4, file) < 0)
        goto out;

    result = OK;

out:

    return (result);
}

int
fgetc(FILE *stream) {
    int result = EOF;
    struct _clib4 *__clib4 = __CLIB4;
    ENTER();
    SHOWPOINTER(stream);

    assert(stream != NULL);

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");
        __set_errno_r(__clib4, EFAULT);

        RETURN(result);
        return result;
    }

    __check_abort_f(__clib4);

    __flockfile_r(__clib4, stream);

    if (__fgetc_check(__clib4, stream) < 0)
        goto out;

    result = __getc(__clib4, stream);

out:
    __funlockfile_r(__clib4, stream);

    LEAVE();
    return (result);
}
