/*
 * $Id: usergroup_getuid.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

uid_t getuid(void) {
    uid_t result;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    if (__clib2->__root_mode) {
        result = __clib2->__root_uid;
    } else {
        result = __getuid();
    }

    __check_abort();

    RETURN(result);
    return (result);
}
