/*
 * $Id: stdlib_termination_message.c,v 1.6 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
__print_termination_message(const char *termination_message) {
    static BOOL termination_message_printed = FALSE;
    struct _clib2 *__clib2 = __CLIB2;

    if (NOT termination_message_printed) {
        termination_message_printed = TRUE;

        if (NOT __clib2->__no_standard_io && __clib2->__num_iob > STDERR_FILENO) {
            fputs((termination_message != NULL) ? termination_message : "Abnormal program termination", stderr);
            fputc('\n', stderr);
        }
    }
}
