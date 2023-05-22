/*
 * $Id: usergroup_getpass.c,v 1.4 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

char *
getpass(const char *prompt) {
    char *result = NULL;

    ENTER();

    SHOWSTRING(prompt);

    assert(prompt != NULL);

    if (prompt == NULL) {
        SHOWMSG("invalid prompt");

        __set_errno(EFAULT);
        goto out;
    }

    result = __getpass((STRPTR) prompt);

    SHOWSTRING(result);

out:

    __check_abort();

    RETURN(result);
    return (result);
}
