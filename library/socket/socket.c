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
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    SHOWVALUE(domain);
    SHOWVALUE(type);
    SHOWVALUE(protocol);

    __stdio_lock(__clib2);

    fd_slot_number = __find_vacant_fd_entry(__clib2);
    if (fd_slot_number < 0) {
        if (__grow_fd_table(__clib2, 0) < 0) {
            SHOWMSG("couldn't find a vacant fd slot and no memory to create one");
            goto out;
        }

        fd_slot_number = __find_vacant_fd_entry(__clib2);
        assert(fd_slot_number >= 0);
    }

    lock = __create_semaphore();
    if (lock == NULL) {
        __set_errno(ENOMEM);
        goto out;
    }

    /* Roadshow is based upon the 4.4BSD-Lite2 TCP/IP stack which uses raw sockets for ICMP operations.
     * - Olaf Barthel
     */
    if (type == SOCK_DGRAM && protocol == IPPROTO_ICMP)
        type = SOCK_RAW;

    /* Force AF_LOCAL to be AF_INET otherwise Roadshow calls will fail */
    if (domain == AF_LOCAL) {
        domain = AF_INET;
        type == SOCK_DGRAM;
    }

    socket_fd = __socket(domain, type, protocol);
    Printf("socket_fd = %d\n", socket_fd);
    if (socket_fd < 0) {
        SHOWMSG("could not create socket");
        goto out;
    }

    fd = __clib2->__fd[fd_slot_number];

    __initialize_fd(fd, __socket_hook_entry, (BPTR) socket_fd, FDF_IN_USE | FDF_IS_SOCKET | FDF_READ | FDF_WRITE, lock);

    lock = NULL;

    result = fd_slot_number;

out:

    __stdio_unlock(__clib2);
    __delete_semaphore(lock);

    __check_abort();

    RETURN(result);
    return (result);
}
