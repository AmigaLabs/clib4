/*
 * $Id: usergroup_getpgrp.c,v 1.0 2021-02-14 18:17:00 clib4devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

pid_t
getpgrp(void) {
    pid_t result;

    ENTER();

    /* No user database: this process is on its own, so it is its own group. */
    CHECK_USERGROUP_LIBRARY(0);

    result = __getpgrp();

    __check_abort();

    RETURN(result);
    return (result);
}
