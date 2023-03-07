/*
 * $Id: unistd_setcurrentpath.c,v 1.6 2023-03-05 12:04:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
__set_current_path(const char *path_name) {
    int result = ERROR;

    SHOWSTRING(path_name);

    ENTER();

    assert(path_name != NULL);

    if (strlen(path_name) + 1 > sizeof(__current_path_name)) {
        SHOWMSG("path is too long");

        __set_errno(ENAMETOOLONG);
        goto out;
    }

    /* Only store the path if it's absolute. */
    if (__unix_path_semantics) {
        if (path_name[0] == '/')
            strcpy(__current_path_name, path_name);
        else
            strcpy(__current_path_name, "");
    } else {
        if (strchr(path_name, ':'))
            strcpy(__current_path_name, path_name);
        else
            strcpy(__current_path_name, "");

    }

    result = OK;

out:

    RETURN(result);
    return (result);
}
