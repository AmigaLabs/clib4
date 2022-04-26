/*
 * $Id: socket_socket.c,v 1.10 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
socket(int domain, int type, int protocol) {
    struct SignalSemaphore *lock = NULL;
    int result = ERROR;
    struct fd *fd;
    int fd_slot_number;
    LONG socket_fd;

    ENTER();

    SHOWVALUE(domain);
    SHOWVALUE(type);
    SHOWVALUE(protocol);

    __stdio_lock();

    fd_slot_number = __find_vacant_fd_entry();
    if (fd_slot_number < 0) {
        if (__grow_fd_table(0) < 0) {
            SHOWMSG("couldn't find a vacant fd slot and no memory to create one");
            goto out;
        }

        fd_slot_number = __find_vacant_fd_entry();
        assert(fd_slot_number >= 0);
    }

    lock = __create_semaphore();
    if (lock == NULL) {
        __set_errno(ENOMEM);
        goto out;
    }

    socket_fd = __socket(domain, type, protocol);
    if (socket_fd < 0) {
        SHOWMSG("could not create socket");
        goto out;
    }

    fd = __fd[fd_slot_number];

    __initialize_fd(fd, __socket_hook_entry, (BPTR) socket_fd, FDF_IN_USE | FDF_IS_SOCKET | FDF_READ | FDF_WRITE, lock);

    lock = NULL;

    result = fd_slot_number;

out:

    __stdio_unlock();
    __delete_semaphore(lock);

    if (__check_abort_enabled)
        __check_abort();

    RETURN(result);
    return (result);
}
