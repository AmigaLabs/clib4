/*
 * $Id: usergroup_setgroups.c,v 1.5 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

int setgroups(int ngroups, const gid_t *groups) {
    int result = ERROR;

    ENTER();

    SHOWVALUE(ngroups);
    SHOWPOINTER(groups);

    assert(ngroups == 0 || groups != NULL);
    assert(__UserGroupBase != NULL);

    if (ngroups != 0 && groups == NULL) {
        SHOWMSG("invalid groups parameter");

        __set_errno(EFAULT);
        goto out;
    }

    if (ngroups > 0) {
        result = __setgroups(ngroups, (LONG *) groups);
    } else {
        result = OK;
    }

out:

    __check_abort();

    RETURN(result);
    return (result);
}
