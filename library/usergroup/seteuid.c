/*
 * $Id: usergroup_seteuid.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

int seteuid(uid_t u) {
    int result;

    ENTER();

    SHOWVALUE(u);

    result = setreuid((ULONG) - 1, (ULONG) u);

    __check_abort();

    RETURN(result);
    return (result);
}
