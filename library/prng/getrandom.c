/*
 * $Id: prng_getrandom.c,v 1.0 2023-06-15 12:04:22 clib4devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <sys/random.h>

/* Set BUFFER (of size LENGTH) to random bytes under the control of FLAGS.
   Return the number of bytes written (> 0).
   Upon error, return -1 and set errno.  */
ssize_t
getrandom(void *buffer, size_t length, unsigned int flags) {
    struct _clib4 *__clib4 = __CLIB4;
    BOOL devrandom = (flags & GRND_RANDOM) != 0;
    int fd = __clib4->randfd[devrandom];

    if (fd < 0) {
        int oflags = (O_RDONLY + (flags & GRND_NONBLOCK ? O_NONBLOCK : 0));
        fd = open(RANDOMDEV, oflags);
        if (fd < 0) {
            if (errno == ENOENT || errno == ENOTDIR)
                __set_errno(ENOSYS);
            return -1;
        }
        __clib4->randfd[devrandom] = fd;
    }

    return read(fd, buffer, length);
}