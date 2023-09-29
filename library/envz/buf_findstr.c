/*
 * $Id: envz__buf_findstr.c,v 1.0 2023-04-16 12:04:22 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#include "buf_findstr.h"

/* Find string str in buffer buf of length buf_len.  Point buf to character after string,
   or set it to NULL if end of buffer is reached.  Return 1 if found, 0 if not. */
int
_buf_findstr(const char *str, char **buf, size_t *buf_len) {
    int i = 0;
    int j = 0;

    for (i = 0; i < *buf_len; i++) {
        if (str[0] == (*buf)[i]) {
            j = i;
            while (str[j - i] && (str[j - i] == (*buf)[j])) j++;
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
