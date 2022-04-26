/*
 * $Id: signal_sigfillset.c,v 1.0 2022-02-16 20:25:00 clib2devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

int
sigfillset(sigset_t *set) {
    ENTER();

    SHOWPOINTER(set);

    *set = ~((sigset_t) 0);

    RETURN(0);
    return 0;
}