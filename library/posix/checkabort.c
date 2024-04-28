/*
 * $Id: signal_checkabort.c,v 1.4 2006-04-05 08:39:45 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
__check_abort(void) {
    struct _clib4 *__clib4 = __CLIB4;

    if (__clib4->__check_abort_enabled && CheckSignal(__clib4->__break_signal_mask)) {
        raise(SIGINT);
    }
}

/* Faster __check_abort version used when __clib4 is available in the caller function */
void
__check_abort_f(struct _clib4 *__clib4) {
    if (__clib4->__check_abort_enabled && CheckSignal(__clib4->__break_signal_mask)) {
        raise(SIGINT);
    }
}

