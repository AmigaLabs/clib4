/*
 * $Id: unistd_getlogin_r.c,v 1.1 2023-04-13 09:51:53 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

int
getlogin_r(char *name, size_t size) {
    int i;
    int result = 0;
    LONG len;

    ENTER();

    STRPTR login = __getlogin();
    if (login == NULL) {
        len = GetVar("USER", name, size, 0);
        if (len <= 0)
            len = GetVar("LOGUSER", name, size, 0);
        if (len <= 0)
            len = GetVar("USERNAME", name, size, 0);

        if (len <= 0) {
            if (size > strlen("anonymous")) {
                strcpy(name, "anonymous");
                goto out;
            }
            __set_errno(ERANGE);
            result = -1;
            goto out;
        }

        len = IoErr();  /* Get the real length of the variable */
        if (len >= size) {
            __set_errno(ERANGE);
            result = -1;
            goto out;
        }

        for (i = len - 1; i >= 0; i--) {
            if (name[i] == ' ' || name[i] == '\t' ||
                name[i] == '\r' || name[i] == '\n')
                name[i] = '\0';
            else
                break;
        }
    }
    else
        strncpy(name, login, size);

    __check_abort();

out:
    RETURN(result);
    return result;
}