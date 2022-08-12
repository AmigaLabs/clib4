/*
 * $Id: usergroup_setreuid.c,v 1.4 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

int
setreuid(uid_t real, uid_t eff) {
    int result;

    ENTER();

    SHOWVALUE(real);
    SHOWVALUE(eff);

    assert(__UserGroupBase != NULL);

    if (__root_mode) {
        if (real != (uid_t) - 1)
            __root_uid = real;

        if (eff != (uid_t) - 1)
            __root_euid = eff;

        result = OK;
    } else {
        result = __setreuid((LONG) real, (LONG) eff);
    }

    __check_abort();

    RETURN(result);
    return (result);
}
