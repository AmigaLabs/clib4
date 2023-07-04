/*
 * $Id: usergroup_getgrent.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

#ifdef getgrent
#undef getgrent
#endif /* getgrent */

struct group *
getgrent(void) {
    struct group *result;

    ENTER();

    result = __getgrent();

    __check_abort();

    RETURN(result);
    return (result);
}
