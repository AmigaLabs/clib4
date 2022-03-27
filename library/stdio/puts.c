/*
 * $Id: stdio_puts.c,v 1.9 2022-03-27 12:04:25 clib2devs Exp $
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

    ENTER();

    SHOWSTRING(s);

    assert(s != NULL);

    if (__check_abort_enabled)
        __check_abort();

    flockfile(stream);

    if (s == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    assert(__is_valid_iob(file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    /* If buffering is disabled for an interactive stream
   switch to line buffering to improve the readability of
   the output: instead of pumping out the entire buffer
   break it up into individual lines. */
    buffer_mode = (file->iob_Flags & IOBF_BUFFER_MODE);
    if (buffer_mode == IOBF_BUFFER_MODE_NONE) {
        struct fd *fd = __fd[file->iob_Descriptor];

        __fd_lock(fd);

        if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_INTERACTIVE))
            buffer_mode = IOBF_BUFFER_MODE_LINE;

        __fd_unlock(fd);
    }

    if (__fputc_check(stream) < 0)
        goto out;

    while ((c = (*s++)) != '\0') {
        if (__putc(c, stream, buffer_mode) == EOF)
            goto out;
    }

    if (__putc('\n', stream, buffer_mode) == EOF)
        goto out;

    result = OK;

out:

    /* Note: if buffering is disabled for this stream, then we still
       may have buffered data around, queued to be printed right now.
       This is intended to improve performance as it takes more effort
       to write a single character to a file than to write a bunch. */
    if (result == 0 && (file->iob_Flags & IOBF_BUFFER_MODE) == IOBF_BUFFER_MODE_NONE) {
        if (__iob_write_buffer_is_valid(file) && __flush_iob_write_buffer(file) < 0) {
            SHOWMSG("couldn't flush the write buffer");
            result = EOF;
        }
    }

    funlockfile(stream);

    RETURN(result);
    return (result);
}
