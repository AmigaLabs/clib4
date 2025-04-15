/*
 * $Id: unistd_setcurrentpath.c,v 1.6 2023-03-05 12:04:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
__set_current_path(const char *path_name) {
    int result = ERROR;
    struct _clib4 *__clib4 = __CLIB4;

    SHOWSTRING(path_name);

    ENTER();

    assert(path_name != NULL);

    if (strlen(path_name) + 1 > sizeof(__clib4->__current_path_name)) {
        SHOWMSG("path is too long");

        __set_errno(ENAMETOOLONG);
        goto out;
    }

    /* Only store the path if it's absolute. */
    if (__clib4->__unix_path_semantics) {
        if (path_name[0] == '/')
            strcpy(__clib4->__current_path_name, path_name);
        else
            strcpy(__clib4->__current_path_name, "");
    } else {
        if (strchr(path_name, ':'))
            strcpy(__clib4->__current_path_name, path_name);
        else
            strcpy(__clib4->__current_path_name, "");
    }
    D(("Current path: %s", __clib4->__current_path_name));

    result = OK;

out:

    RETURN(result);
    return (result);
}
