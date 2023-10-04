/*
 * $Id: signal_sigblock.c,v 1.4 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

int
sigblock(int signal_mask) {
    int result;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(signal_mask);

    result = __clib4->__signals_blocked;

    __clib4->__signals_blocked |= signal_mask;

    RETURN(result);
    return (result);
}
