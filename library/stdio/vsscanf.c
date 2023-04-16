/*
 * $Id: stdio_vsscanf.c,v 1.2 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
vsscanf(const char *s, const char *format, va_list arg) {
    struct iob string_iob;
    char local_buffer[32];
    int result = EOF;

    ENTER();

    SHOWSTRING(s);
    SHOWSTRING(format);

    assert(s != NULL && format != NULL);

    __check_abort();

    if (s == NULL || format == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    __initialize_iob(&string_iob, __sscanf_hook_entry,
                     NULL,
                     local_buffer, sizeof(local_buffer),
                     -1,
                     -1,
                     IOBF_IN_USE | IOBF_READ | IOBF_BUFFER_MODE_FULL | IOBF_INTERNAL,
                     NULL);

    string_iob.iob_String = (STRPTR) s;
    string_iob.iob_StringLength = strlen(s);

    result = vfscanf((FILE * ) &string_iob, format, arg);

out:

    RETURN(result);
    return (result);
}
