/*
 * $Id: argz_create_sep.c,v 1.0 2021-01-21 11:14:32 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <argz.h>

error_t
argz_create_sep(const char *string, int sep, char **argz, size_t *argz_len) {
    size_t len;
    int i;
    int num_strings = 0;
    char delim[2];
    char *running;
    char *old_running;
    char *token;
    char *iter;

    delim[0] = (char) sep;
    delim[1] = '\0';

    running = strdup(string);

    if (!running) {
        return ENOMEM;
    }

    old_running = running;

    while ((token = strsep(&running, delim))) {
        len = strlen(token);
        *argz_len += (len + 1);
        num_strings++;
    }

    if (!(*argz = (char *) malloc(*argz_len))) {
        free(old_running);
        return ENOMEM;
    }

    free(old_running);

    running = strdup(string);

    if (!running) {
        free(*argz);
        return ENOMEM;
    }

    old_running = running;

    iter = *argz;
    for (i = 0; i < num_strings; i++) {
        token = strsep(&running, delim);
        len = strlen(token) + 1;
        memcpy(iter, token, len);
        iter += len;
    }

    free(old_running);
    return 0;
}
