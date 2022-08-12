/*
 * $Id: usergroup_setsid.c,v 1.4 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

pid_t setsid(void) {
    long result;

    ENTER();

    assert(__UserGroupBase != NULL);

    result = (long) __setsid();

    __check_abort();

    RETURN(result);
    return (result);
}
