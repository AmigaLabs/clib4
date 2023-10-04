/*
 * $Id: unistd_pause.c,v 1.0 2022-03-15 08:39:45 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int pause(void) {

    Wait(SIGBREAKF_CTRL_C);
    raise(SIGINT);

    return 0;
}