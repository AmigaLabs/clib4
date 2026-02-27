/*
 * $Id: signal_signal.c,v 1.4 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

void (*signal(int sig, void (*handler)(int)))(int) {
    int table_entry = sig - SIGHUP;
    void (*result)(int) = SIG_ERR;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(sig);
    SHOWPOINTER(handler);

    if (sig < 0 || sig > NSIG || handler == SIG_ERR) {
        SHOWMSG("unsupported signal");

        __set_errno_r(__clib4, EINVAL);
        goto out;
    }

    if (table_entry < 0)
        table_entry = 0;

    result = (void (*)(int)) __clib4->__signal_handler_table[table_entry];

    __clib4->__signal_handler_table[table_entry] = (signal_handler_t) handler;

out:

    RETURN(result);
    return (result);
}
