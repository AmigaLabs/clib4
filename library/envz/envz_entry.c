/*
 * $Id: envz_envz_entry.c,v 1.0 2023-04-16 12:04:22 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#include <argz.h>
#include <envz.h>

#include "buf_findstr.h"

char *
envz_entry(const char *envz, size_t envz_len, const char *name) {
    char *buf_ptr = (char *) envz;
    size_t buf_len = envz_len;

    while (buf_len) {
        if (_buf_findstr(name, &buf_ptr, &buf_len)) {
            if (buf_ptr) {
                if (*buf_ptr == '=' || *buf_ptr == '\0') {
                    buf_ptr--;

                    /* Move buf_ptr back to start of entry. */
                    while (*buf_ptr != '\0' && buf_ptr != envz) buf_ptr--;

                    if (*buf_ptr == '\0')
                        buf_ptr++;

                    return (char *) buf_ptr;
                }
            }
        }
    }
    return 0;
}
