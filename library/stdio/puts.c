/*
 * $Id: stdio_puts.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * puts() — newlib-inspired rewrite. Writes string + newline to stdout.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
puts(const char *s) {
    struct _clib4 *__clib4 = __CLIB4;
    FILE *stream = __stdout_r(__clib4);
    int result;

    ENTER();

    if (s == NULL || stream == NULL) {
        __set_errno_r(__clib4, EFAULT);
        RETURN(EOF);
        return EOF;
    }

    /* fputs + newline */
    result = __fputs_r(__clib4, s, stream);
    if (result == EOF) {
        RETURN(result);
        return result;
    }

    /* Append newline. */
    __flockfile_r(__clib4, stream);
    {
        struct iob *fp = (struct iob *) stream;
        int buffer_mode = (fp->iob_Flags & IOBF_BUFFER_MODE);

        if (__fputc(__clib4, '\n', stream, buffer_mode) == EOF)
            result = EOF;
    }
    __funlockfile_r(__clib4, stream);

    RETURN(result);
    return result;
}
