/*
 * $Id: usergroup_setregid.c,v 1.4 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

int setregid(gid_t real, gid_t eff) {
    int result;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    SHOWVALUE(real);
    SHOWVALUE(eff);

    if (__clib2->__root_mode) {
        if (real != (gid_t) - 1)
            __clib2->__root_gid = real;

        if (eff != (gid_t) - 1)
            __clib2->__root_egid = eff;

        result = OK;
    } else {
        result = __setregid((LONG) real, (LONG) eff);
    }

    __check_abort();

    RETURN(result);
    return (result);
}
