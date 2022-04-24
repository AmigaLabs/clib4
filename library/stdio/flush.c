/*
 * $Id: stdio_flush.c,v 1.5 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/* This is roughly equivalent to fflush(), but returns the last character
   written to the buffer, or EOF if flushing produced an error. The putc()
   macro and the internal __putc() macro need this functionality. This
   function is called only if the last character written to the buffer
   was a line feed, prompting the buffer contents to be flushed. It should
   never be used in place of fflush(). */
int
__flush(FILE *stream) {
    struct iob *iob = (struct iob *) stream;
    int result = EOF;
    int last_c;

    ENTER();

    SHOWPOINTER(stream);

    assert(stream != NULL);

    flockfile(stream);

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");

        __set_errno(EFAULT);
        goto out;
    }

    assert(__is_valid_iob(iob));
    assert(iob->iob_BufferWriteBytes > 0);
    assert(iob->iob_BufferSize > 0);

    last_c = iob->iob_Buffer[iob->iob_BufferWriteBytes - 1];

    if (__flush_iob_write_buffer(iob) < 0) {
        /* Remove the last character stored in the buffer, which is
           typically a '\n'. */
        iob->iob_BufferWriteBytes--;
        goto out;
    }

    result = last_c;

out:

    funlockfile(stream);

    RETURN(result);
    return (result);
}
