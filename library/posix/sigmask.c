/*
 * $Id: signal_sigmask.c,v 1.5 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

int
sigmask(int sig) {
    int result;

    ENTER();

    SHOWVALUE(sig);

    assert(0 <= sig && sig <= 31);

    if (SIGHUP <= sig && sig <= SIGUSR2)
        result = (1 << sig);
    else
        result = 0;

    RETURN(result);
    return (result);
}
