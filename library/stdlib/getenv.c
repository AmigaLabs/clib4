/*
 * $Id: stdlib_getenv.c,v 1.5 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

char *
getenv_r(struct _clib4 *__clib4, const char *name, int *offset) {
    static char env_var_buffer[FILENAME_MAX] = {0};
    char *result = NULL;
    char ***p_environ = &__clib4->__environment;
    register int len;
    register char **p;
    const char *c;

    ENTER();

    SHOWSTRING(name);

    assert(name != NULL);

    if (name == NULL) {
        SHOWMSG("invalid name parameter");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }


    if (!*p_environ) {
        __set_errno_r(__clib4, EFAULT);
        SHOWMSG("couldn't get the variable");
        goto out;
    }

    MutexObtain(__clib4->__environment_lock);

    c = name;
    len = 0;
    while (*c && *c != '=') {
        c++;
        len++;
    }

    for (p = *p_environ; *p; ++p) {
        if (!strncmp (*p, name, len)) {
            if (*(c = *p + len) == '=') {
                *offset = p - *p_environ;
                result = (char *) (++c);
                break;
            }
        }
    }

    MutexRelease(__clib4->__environment_lock);

out:

    RETURN(result);
    return (result);
}

char *
getenv(const char *name) {
    struct _clib4 *__clib4 = __CLIB4;
    int offset;

    return getenv_r(__clib4, name, &offset);
}