/*
 * $Id: fcntl_creat.c,v 1.6 2006-01-08 12:04:22 clib4devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

int
creat(const char *path_name, mode_t mode) {
    int result = ERROR;

    ENTER();

    SHOWSTRING(path_name);
    SHOWVALUE(mode);

    __check_abort();

    assert(path_name != NULL);

    if (path_name == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    result = open(path_name, O_WRONLY | O_CREAT | O_TRUNC, mode);

out:

    RETURN(result);
    return (result);
}
