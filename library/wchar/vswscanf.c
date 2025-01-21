/*
 * $Id: wchar_vswscanf.c,v 1.0 2022-03-04 16:29:27 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
vswscanf(const wchar_t *s, const wchar_t *format, va_list arg) {
    struct iob string_iob;
    int result = EOF;
    int size = wcslen(s) * sizeof (wchar_t);
    char buffer[size];

    ENTER();

    SHOWWSTRING(s);
    SHOWWSTRING(format);

    assert(s != NULL && format != NULL);

    __check_abort();

    if (format == NULL || s == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }
    wcstombs(buffer, s, size);
    __initialize_iob(&string_iob,
                     __sscanf_hook_entry,
                     NULL,
                     buffer,
                     size,
                     -1,
                     -1,
                     IOBF_IN_USE | IOBF_READ | IOBF_BUFFER_MODE_FULL | IOBF_INTERNAL,
                     NULL);

    string_iob.iob_BufferReadBytes = string_iob.iob_BufferSize;

    result = vfwscanf((FILE * ) & string_iob, format, arg);

out:
    RETURN(result);
    return result;
}