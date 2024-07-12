/*
 * $Id: argz_replace.c,v 1.0 2021-01-21 11:38:20 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <argz.h>

/* Find string str in buffer buf of length buf_len.  Point buf to character after string,
   or set it to NULL if end of buffer is reached.  Return 1 if found, 0 if not. */
static int
_buf_findstr(const char *str, char **buf, size_t *buf_len) {
    size_t i = 0;
    size_t j = 0;

    for (i = 0; i < *buf_len; i++) {
        if (str[0] == (*buf)[i]) {
            j = i;
            while (str[j - i] && (str[j - i] == (*buf)[j]))
                j++;
            if (str[j - i] == '\0') {
                *buf += j;
                *buf_len -= j;
                return 1;
            }
        }
    }

    if (i == *buf_len) {
        *buf += *buf_len;
        *buf_len = 0;
    }

    return 0;
}

error_t
argz_replace(char **argz, size_t *argz_len, const char *str, const char *with, unsigned *replace_count) {
    const size_t str_len = strlen(str);
    const size_t with_len = strlen(with);
    const size_t len_diff = with_len - str_len;

    char *buf_iter = *argz;
    size_t buf_len = *argz_len;
    char *last_iter = NULL;
    char *new_argz = NULL;
    size_t new_argz_len = 0;
    char *new_argz_iter = NULL;

    *replace_count = 0;
    new_argz_len = *argz_len;

    while (buf_len) {
        if (_buf_findstr(str, &buf_iter, &buf_len)) {
            *replace_count += 1;
            new_argz_len += len_diff;
        }
    }

    if (*replace_count) {
        new_argz = (char *) malloc(new_argz_len);

        buf_iter = *argz;
        buf_len = *argz_len;
        last_iter = buf_iter;
        new_argz_iter = new_argz;

        while (buf_len) {
            if (_buf_findstr(str, &buf_iter, &buf_len)) {
                /* copy everything up to, but not including str, from old argz to
                 new argz. */
                memcpy(new_argz_iter, last_iter, buf_iter - last_iter - str_len);
                new_argz_iter += (buf_iter - last_iter - str_len);
                /* copy replacement string. */
                memcpy(new_argz_iter, with, with_len);
                new_argz_iter += with_len;
                last_iter = buf_iter;
            }
        }
        /* copy everything after last occurrence of str. */
        memcpy(new_argz_iter, last_iter, (size_t)(*argz + *argz_len - last_iter));

        /* reallocate argz, and copy over the new value. */
        if (!(*argz = (char *) realloc(*argz, new_argz_len)))
            return ENOMEM;

        memcpy(*argz, new_argz, new_argz_len);
        *argz_len = new_argz_len;

        if (*argz_len == 0) {
            free(*argz);
            *argz = NULL;
        }
        free(new_argz);
    }

    return 0;
}