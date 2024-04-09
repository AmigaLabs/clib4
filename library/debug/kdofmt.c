/*
 * $Id: debug_kdofmt.c,v 1.4 2024-04-09 12:04:22 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "debug_headers.h"

VOID
KDoFmt(const char *format_string, APTR data_stream, APTR put_char_routine, APTR put_char_data) {
    if (format_string != NULL)
        RawDoFmt((STRPTR) format_string, data_stream, (VOID (*)()) put_char_routine, put_char_data);
}
