/*
 * $Id: unistd_getentropy.c,v 1.0 2023-06-15 12:04:27 clib4devs Exp $
*/

#define _DEFAULT_SOURCE
#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/random.h>

int
getentropy(void *buffer, size_t len) {
    char *pos = buffer;

    if (len > 256) {
        __set_errno(EIO);
        return -1;
    }

    if (buffer == NULL && len > 0) {
        __set_errno(EFAULT);
        return -1;
    }

    while (len) {
        ssize_t ret = getrandom(pos, len, 0);

        if (ret < 0) {
            if (__get_errno() == EINTR)
                continue;

            return -1;
        }

        /*
         * getrandom() never reports zero bytes without an error, but do not
         * take that for granted: making no progress here would turn this
         * loop into an endless one.
         */
        if (ret == 0) {
            __set_errno(EIO);
            return -1;
        }

        pos += ret;
        len -= (size_t) ret;
    }

    return 0;
}
