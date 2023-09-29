/*
 * $Id: usergroup_setpwent.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

void setpwent(void) {
    ENTER();

    __setpwent();

    __check_abort();

    LEAVE();
}
