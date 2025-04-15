/*
 * $Id: fcntl_flock.c,v 1.0 2022-08-21 12:04:22 clib4devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

int
flock(int fd, int op) {
    int ret = -1;
    struct flock lock;

    if (op != LOCK_SH
        && op != LOCK_EX
        && op != LOCK_UN
        && op != (LOCK_SH|LOCK_NB)
        && op != (LOCK_EX|LOCK_NB)) {
        __set_errno(EINVAL);
        goto out;
    }
    /* Initialize the flock structure. */
    memset(&lock, 0, sizeof(lock));

    if (op & LOCK_SH) {
        lock.l_type = F_RDLCK;
        /* Place a shared (read) lock on the file. */
        ret = fcntl(fd, F_SETLK, &lock);
    }
    else if (op & LOCK_EX) {
        lock.l_type = F_WRLCK;
        /* Place an exclusive (write) lock on the file. */
        ret = fcntl(fd, F_SETLK, &lock);
    }
    else if (op == LOCK_UN) {
        /* Release the lock. */
        lock.l_type = F_UNLCK;
        ret = fcntl (fd, F_SETLKW, &lock);
    }

out:

    return ret;
}
