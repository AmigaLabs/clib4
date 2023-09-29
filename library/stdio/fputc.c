/*
 * $Id: stdio_fputc.c,v 1.8 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
__fputc_check(FILE *stream) {
    struct iob *file = (struct iob *) stream;
    int result = EOF;
    struct _clib4 *__clib4 = __CLIB4;

    assert(stream != NULL);

    if (stream == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    assert(__is_valid_iob(__clib4, file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE)) {
        SHOWMSG("this file is not even in use");

        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno(EBADF);

        goto out;
    }

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_WRITE)) {
        SHOWMSG("this stream is not write enabled");

        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno(EBADF);

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
__fputc(int c, FILE *stream, int buffer_mode) {
    struct iob *file = (struct iob *) stream;
    int result = EOF;
    struct _clib4 *__clib4 = __CLIB4;

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

    ENTER();
    SHOWVALUE(c);
    SHOWPOINTER(stream);

    assert(stream != NULL);

    flockfile(stream);

    if (__fputc_check(stream) < 0)
        goto out;

    /* TODO - We have to investigate while stdout reach this point with IOBF_BUFFER_MODE_NONE when it is
     * initialized with IOBF_BUFFER_MODE_LINE
     */

    /* Using no buffer with fputc is really slow. It no buffer is set, change filt to Line Mode buffering */
    if (FLAG_IS_SET(file->iob_Flags, IOBF_BUFFER_MODE_NONE))
        SET_FLAG(file->iob_Flags, IOBF_BUFFER_MODE_LINE);

    result = __fputc(c, stream, (file->iob_Flags & IOBF_BUFFER_MODE));

out:

    funlockfile(stream);

    RETURN(result);
    return (result);
}
