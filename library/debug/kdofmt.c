/*
 * $Id: debug_kdofmt.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#include "debug_headers.h"

VOID
KDoFmt(const char *format_string, APTR data_stream, APTR put_char_routine, APTR put_char_data) {
    if (format_string != NULL)
        RawDoFmt((STRPTR) format_string, data_stream, (VOID (*)()) put_char_routine, put_char_data);
}
