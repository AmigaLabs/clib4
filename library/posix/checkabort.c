/*
 * $Id: signal_checkabort.c,v 1.4 2006-04-05 08:39:45 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
__check_abort(void) {
    struct _clib2 *__clib2 = __CLIB2;

    if (__clib2->__check_abort_enabled && CheckSignal(__clib2->__break_signal_mask)) {
        raise(SIGINT);
    }
}
