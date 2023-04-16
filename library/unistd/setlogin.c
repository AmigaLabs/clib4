/*
 * $Id: unistd_setlogin.c,v 1.0 2023-04-13 09:51:53 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

int
setlogin(const char *name) {
    ENTER();

    assert(__UserGroupBase != NULL);

    int result = __setlogin(name);

    __check_abort();

    RETURN(result);
    return result;
}