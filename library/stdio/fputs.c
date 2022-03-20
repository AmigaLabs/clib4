/*
 * $Id: stdio_fputs.c,v 1.7 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
fputs(const char *s, FILE *stream) {
    struct iob *file = (struct iob *) stream;
    int result = EOF;
    int buffer_mode;
    int c;

    ENTER();

    SHOWSTRING(s);
    SHOWPOINTER(stream);

    assert(s != NULL && stream != NULL);

    if (__check_abort_enabled)
        __check_abort();

    if (s == NULL || stream == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    flockfile(stream);

    assert(__is_valid_iob(file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    buffer_mode = (file->iob_Flags & IOBF_BUFFER_MODE);
    if (buffer_mode == IOBF_BUFFER_MODE_NONE)
        buffer_mode = IOBF_BUFFER_MODE_LINE;

    if (__fputc_check(stream) < 0)
        goto out;

    while ((c = (*s++)) != '\0') {
        if (__putc(c, stream, buffer_mode) == EOF)
            goto out;
    }

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
