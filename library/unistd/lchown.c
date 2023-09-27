/*
 * $Id: unistd_lchown.c,v 1.7 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
lchown(const char *path_name, uid_t owner, gid_t group) {
    int result = ERROR;

    ENTER();

    SHOWSTRING(path_name);
    SHOWVALUE(owner);
    SHOWVALUE(group);

    assert(path_name != NULL);

    __check_abort();

    if (path_name == NULL) {
        SHOWMSG("invalid path name");

        __set_errno(EFAULT);
        goto out;
    }

    result = chown(path_name, owner, group);

out:

    RETURN(result);
    return (result);
}
