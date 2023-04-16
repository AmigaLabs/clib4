/*
 * $Id: envz_envz_add.c,v 1.0 2023-04-16 12:04:22 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#include <argz.h>
#include <envz.h>

error_t
envz_add(char **envz, size_t *envz_len, const char *name, const char *value) {
    char *concat = NULL;
    int name_len = 0;
    int val_len = 0;
    int retval = 0;

    envz_remove(envz, envz_len, name);

    if (value) {
        name_len = strlen(name);
        val_len = strlen(value);
        if (!(concat = (char *) malloc(name_len + val_len + 2)))
            return ENOMEM;

        memcpy(concat, name, name_len);
        concat[name_len] = '=';
        memcpy(concat + name_len + 1, value, val_len + 1);

        retval = argz_add(envz, envz_len, concat);
        free(concat);
    } else {
        retval = argz_add(envz, envz_len, name);
    }
    return retval;
}
