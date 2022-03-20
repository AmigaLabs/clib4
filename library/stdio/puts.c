/*
 * $Id: stdio_puts.c,v 1.8 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
puts(const char *s) {
    struct iob *file = (struct iob *) stdout;
    int result = EOF;
    int buffer_mode;
    int c;

    ENTER();

    SHOWSTRING(s);

    assert(s != NULL);

    if (__check_abort_enabled)
        __check_abort();

    flockfile(stdout);

    if (s == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    assert(__is_valid_iob(file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    buffer_mode = (file->iob_Flags & IOBF_BUFFER_MODE);
    if (buffer_mode == IOBF_BUFFER_MODE_NONE)
        buffer_mode = IOBF_BUFFER_MODE_LINE;

    if (__fputc_check(stdout) < 0)
        goto out;

    while ((c = (*s++)) != '\0') {
        if (__putc(c, stdout, buffer_mode) == EOF)
            goto out;
    }

    if (__putc('\n', stdout, buffer_mode) == EOF)
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

    funlockfile(stdout);

    RETURN(result);
    return (result);
}
