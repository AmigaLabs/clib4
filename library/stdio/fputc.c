/*
 * $Id: stdio_fputc.c,v 1.9 2024-07-20 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
__fputc_check(struct _clib4 *__clib4, FILE *stream) {
    struct iob *file = (struct iob *) stream;
    int result = EOF;

    assert(stream != NULL);

    if (stream == NULL) {
        __set_errno(EBADF);
        goto out;
    }

    assert(__is_valid_iob(__clib4, file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE)) {
        SHOWMSG("this file is not even in use");

        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno_r(__clib4, EBADF);

        goto out;
    }

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_WRITE)) {
        SHOWMSG("this stream is not write enabled");

        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno_r(__clib4, EBADF);

        goto out;
    }

    if (__iob_read_buffer_is_valid(file) && __drop_iob_read_buffer(__clib4, file) < 0) {
        SHOWMSG("couldn't get rid of the read buffer.");
        goto out;
    }

    result = OK;

out:

    return (result);
}

int
__fputc(struct _clib4 *__clib4, int c, FILE *stream, int buffer_mode) {
    struct iob *file = (struct iob *) stream;
    int result = EOF;

    assert(stream != NULL);

    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    if (__iob_write_buffer_is_full(file) && __flush_iob_write_buffer(__clib4, file) < 0)
        goto out;

    file->iob_Buffer[file->iob_BufferWriteBytes++] = c;

    if ((buffer_mode == IOBF_BUFFER_MODE_NONE || (buffer_mode == IOBF_BUFFER_MODE_LINE && c == '\n')) && __flush_iob_write_buffer(__clib4, file) < 0) {
        /* Pretend that the last character was not written. */
        file->iob_BufferWriteBytes--;
        goto out;
    }

    /* Clip everything but the least significant eight bits. This
       also takes care of the sign. Thus, a -1 (== EOF) always comes
       out as 255. */
    result = (c & 255);

out:

    return (result);
}

int
fputc(int c, FILE *stream) {
    struct iob *file = (struct iob *) stream;
    int result = EOF;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();
    SHOWVALUE(c);
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

    if (__fputc_check(__clib4, stream) < 0)
        goto out;

    result = __fputc(__clib4, c, stream, (file->iob_Flags & IOBF_BUFFER_MODE));

out:

    SHOWMSG("__funlockfile_r");
    __funlockfile_r(__clib4, stream);

    RETURN(result);
    return (result);
}
