/*
 * $Id: stdio_vsnprintf.c,v 1.8 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
vsnprintf(char *buffer, size_t size, const char *format, va_list arg) {
    struct iob string_iob;
    int result = EOF;
    char local_buffer[32] = {0};

    ENTER();

    SHOWPOINTER(buffer);
    SHOWVALUE(size);
    SHOWSTRING(format);
    SHOWPOINTER(arg);

    assert(format != NULL && arg != NULL && (int) size >= 0);

    if (format == NULL || (buffer == NULL && size > 0)) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }
    memset(buffer, 0, size);

    __initialize_iob(&string_iob, __vsnprintf_hook_entry,
                     NULL,
                     local_buffer, sizeof(local_buffer),
                     -1,
                     -1,
                     IOBF_IN_USE | IOBF_WRITE | IOBF_NO_NUL | IOBF_BUFFER_MODE_NONE | IOBF_INTERNAL,
                     NULL);

    /* Store up to 'size-1' characters in the output buffer. This
       does not include the terminating NUL character */
    string_iob.iob_String = (STRPTR) buffer;
    string_iob.iob_StringSize = (size > 0) ? (size - 1) : 0;

    /* We will return the number of characters that would have been
       stored if there had been enough room. */
    result = vfprintf((FILE * ) &string_iob, format, arg);

    fflush((FILE * ) &string_iob);

out:

    RETURN(result);
    return (result);
}
