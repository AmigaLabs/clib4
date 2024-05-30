/*
 * $Id: usergroup_geteuid.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

/****************************************************************************/

#ifdef geteuid
#undef geteuid
#endif /* geteuid */

uid_t geteuid(void) {
    uid_t result;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    if (__clib4->__root_mode) {
        result = __clib4->__root_euid;
    } else {
        result = __geteuid();
    }

    __check_abort_f(__clib4);

    RETURN(result);
    return (result);
}
