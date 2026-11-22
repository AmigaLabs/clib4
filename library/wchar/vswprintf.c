/*
 * $Id: wchar_vswprintf.c,v 1.4 2024-07-12 12:04:27 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include "wchar_wprintf_core.h"

int
vswprintf(wchar_t *s, size_t l, const wchar_t *fmt, va_list ap) {
    va_list ap2;
    int ret, nl_type[NL_ARGMAX + 1] = {0};
    union arg nl_arg[NL_ARGMAX + 1];
    FOut _out[1];
    struct _clib4 *__clib4 = __CLIB4;

    va_copy(ap2, ap);

    /* Collect the positional arguments ("%1$d"), if the format uses them */
    ret = wprintf_core(__clib4, 0, fmt, &ap2, nl_arg, nl_type);
    if (ret < 0 || (!s) || (!l)) {
        va_end(ap2);
        return ret;
    }
    out_init_buffer(_out, s, l);
    ret = wprintf_core(__clib4, _out, fmt, &ap2, nl_arg, nl_type);

    va_end(ap2);

    if (out_overflow(_out))
        return EOF;
    return ret;
}
