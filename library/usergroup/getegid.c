/*
 * $Id: usergroup_getegid.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

/****************************************************************************/

#ifdef getegid
#undef getegid
#endif /* getegid */

gid_t getegid(void) {
    gid_t result;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    if (__clib2->__root_mode) {
        result = __clib2->__root_egid;
    } else {
        result = __getegid();
    }

    __check_abort();

    RETURN(result);
    return (result);
}
