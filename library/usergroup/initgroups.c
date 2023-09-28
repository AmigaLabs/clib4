/*
 * $Id: usergroup_initgroups.c,v 1.5 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

int
initgroups(const char *name, gid_t basegroup) {
    int result = ERROR;

    ENTER();

    SHOWSTRING(name);
    SHOWVALUE(basegroup);

    assert(name != NULL);

    if (name == NULL) {
        SHOWMSG("invalid name");

        __set_errno(EFAULT);
        goto out;
    }

    result = __initgroups((STRPTR) name, (LONG) basegroup);

out:

    __check_abort();

    RETURN(result);
    return (result);
}
