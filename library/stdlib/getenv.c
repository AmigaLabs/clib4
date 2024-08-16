/*
 * $Id: stdlib_getenv.c,v 1.5 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

char *
getenv(const char *name) {
    static char env_var_buffer[FILENAME_MAX] = {0};
    char *result = NULL;

    ENTER();

    SHOWSTRING(name);

    assert(name != NULL);

    __check_abort();

    if (name == NULL) {
        SHOWMSG("invalid name parameter");

        __set_errno(EFAULT);
        goto out;
    }

    if (GetVar((STRPTR) name, env_var_buffer, sizeof(env_var_buffer), 0) < 0) {
        SHOWMSG("couldn't get the variable");

        __set_errno(__translate_io_error_to_errno(IoErr()));
        goto out;
    }

    result = env_var_buffer;

    SHOWSTRING(result);

out:

    RETURN(result);
    return (result);
}
