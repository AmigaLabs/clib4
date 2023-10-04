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
    int ret = 0;
    char *pos = buffer;

    if (len > 256) {
        __set_errno(EIO);
        return -1;
    }

    while (len) {
        ret = getrandom(pos, len, 0);
        if (ret < 0) {
            if (errno == EINTR)
                continue;
            else
                break;
        }
        pos += ret;
        len -= ret;
        ret = 0;
    }

    return ret;
}
