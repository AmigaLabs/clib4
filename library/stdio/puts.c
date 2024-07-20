/*
 * $Id: stdio_puts.c,v 1.9 2022-03-27 12:04:25 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
puts(const char *s) {
    FILE *stream = stdout;
    struct iob *file = (struct iob *) stream;
    int result = EOF;
    int buffer_mode;
    int c;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(s);

    assert(s != NULL);
    assert(stream != NULL);

    if (s == NULL || stream == NULL) {
        SHOWMSG("invalid stream parameter");
        __set_errno_r(__clib4, EFAULT);

        RETURN(result);
        return (result);
    }

    assert(__is_valid_iob(__clib4, file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    /* If buffering is disabled for an interactive stream
       switch to line buffering to improve the readability of
       the output: instead of pumping out the entire buffer
       break it up into individual lines. */
    buffer_mode = (file->iob_Flags & IOBF_BUFFER_MODE);
    if (buffer_mode == IOBF_BUFFER_MODE_NONE) {
        SHOWMSG("Switching to IOBF_BUFFER_MODE_LINE");
        struct fd *fd = __clib4->__fd[file->iob_Descriptor];

        __fd_lock(fd);

        if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_INTERACTIVE))
            buffer_mode = IOBF_BUFFER_MODE_LINE;

        __fd_unlock(fd);
    }

    __check_abort_f(__clib4);

    __flockfile_r(__clib4, stream);

    if (__fputc_check(__clib4, stream) < 0) {
        __funlockfile_r(__clib4, stream);
        goto out;
    }

    while ((c = (*s++)) != '\0') {
        if (__putc(__clib4, c, stream, buffer_mode) == EOF) {
            __funlockfile_r(__clib4, stream);
            goto out;
        }
    }

    if (__putc(__clib4, '\n', stream, buffer_mode) == EOF) {
        __funlockfile_r(__clib4, stream);
        goto out;
    }

    result = OK;

    __funlockfile_r(__clib4, stream);

out:

    /* Note: if buffering is disabled for this stream, then we still
       may have buffered data around, queued to be printed right now.
       This is intended to improve performance as it takes more effort
       to write a single character to a file than to write a bunch. */
    if (result == OK && (file->iob_Flags & IOBF_BUFFER_MODE) == IOBF_BUFFER_MODE_NONE) {
        SHOWMSG("Flushing write buffer");
        if (__iob_write_buffer_is_valid(file) && __flush_iob_write_buffer(__clib4, file) < 0) {
            SHOWMSG("couldn't flush the write buffer");
            result = EOF;
        }
    }

    RETURN(result);
    return (result);
}
