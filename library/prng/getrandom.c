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

#define GRND_KNOWN_FLAGS (GRND_NONBLOCK | GRND_RANDOM | GRND_INSECURE)

/*
 * Open the RANDOM: stream and remember the descriptor in the per process
 * context so that we do not have to reopen the handler on every call.
 *
 * The descriptor lives in the programme's own descriptor table, which means
 * the programme can close() it, or have it recycled by a later open() or
 * dup2(). Reusing it blindly would make getrandom() read from whatever file
 * happens to sit in that slot and quietly return data that is not random at
 * all, so the cached descriptor is tagged with FDF_IS_RANDOM and validated
 * before every use.
 */
static int
open_random_device(struct _clib4 *__clib4, BOOL devrandom, unsigned int flags) {
    struct fd *fd_entry;
    int oflags;
    int fd;

    fd = __clib4->randfd[devrandom];
    if (fd >= 0) {
        fd_entry = __get_file_descriptor(__clib4, fd);
        if (fd_entry != NULL && FLAG_IS_SET(fd_entry->fd_Flags, FDF_IS_RANDOM))
            return fd;

        /* Not ours any more; forget about it and open a fresh one. */
        SHOWMSG("cached RANDOM: descriptor is stale");
        __clib4->randfd[devrandom] = -1;
    }

    /* O_CLOEXEC keeps this internal descriptor out of the set of file
     * descriptors that are handed down to spawned child processes. */
    oflags = O_RDONLY | O_CLOEXEC | ((flags & GRND_NONBLOCK) ? O_NONBLOCK : 0);

    fd = open(RANDOMDEV, oflags);
    if (fd < 0) {
        if (__get_errno() == ENOENT || __get_errno() == ENOTDIR)
            __set_errno(ENOSYS);

        return -1;
    }

    fd_entry = __get_file_descriptor(__clib4, fd);
    if (fd_entry != NULL)
        SET_FLAG(fd_entry->fd_Flags, FDF_IS_RANDOM);

    __clib4->randfd[devrandom] = fd;

    return fd;
}

/* Set BUFFER (of size LENGTH) to random bytes under the control of FLAGS.
   Return the number of bytes written (> 0).
   Upon error, return -1 and set errno.  */
ssize_t
getrandom(void *buffer, size_t length, unsigned int flags) {
    struct _clib4 *__clib4 = __CLIB4;
    BOOL devrandom;
    ssize_t result;
    int fd;

    ENTER();

    if (flags & ~((unsigned int) GRND_KNOWN_FLAGS)) {
        SHOWMSG("unknown flags");

        __set_errno(EINVAL);
        result = -1;
        goto out;
    }

    if (buffer == NULL && length > 0) {
        __set_errno(EFAULT);
        result = -1;
        goto out;
    }

    if (length == 0) {
        result = 0;
        goto out;
    }

    devrandom = (flags & GRND_RANDOM) != 0;

    fd = open_random_device(__clib4, devrandom, flags);
    if (fd < 0) {
        result = -1;
        goto out;
    }

    result = read(fd, buffer, length);
    if (result == 0) {
        /* The handler reported end of file. There is no such thing as an
         * end of file on a random number generator, so treat this as an
         * I/O error instead of returning zero: callers such as getentropy()
         * would otherwise spin forever waiting for bytes that never come. */
        SHOWMSG("RANDOM: returned EOF");

        __set_errno(EIO);
        result = -1;
    }

out:

    RETURN(result);
    return result;
}
