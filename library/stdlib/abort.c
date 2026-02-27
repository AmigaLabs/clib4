/*
 * $Id: stdlib_abort.c,v 1.5 2006-01-08 12:04:25 clib4devs Exp $
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
__abort(void) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    /* If the signal handler returns it means that we still have
       to terminate the program. */

    __clib4->__check_abort_enabled = FALSE;

    __print_termination_message(NULL);

    LEAVE();

    /* Note that we drop into the exit() function which
       does not trigger the exit trap. */
    _exit(EXIT_FAILURE);
}

void
abort(void) {
    /* abort will not call exit so we have to check if there are some atexit funcions
     * ready to be executed too
     */
    __exit_trap_trigger();

    /* Try to call the signal handler that might be in charge of
       handling cleanup operations, etc. */
    raise(SIGABRT);

    /* If the signal handler returns it means that we still have
       to terminate the program. */
    __abort();
}