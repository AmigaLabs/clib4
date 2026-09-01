/*
 * $Id: socket_socket.c,v 1.10 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include <sys/ioctl.h>

int
socket(int domain, int type, int protocol) {
    APTR lock = NULL;
    int result = ERROR;
    struct fd *fd;
    int fd_slot_number;
    LONG socket_fd;
    int type_flags;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(domain);
    SHOWVALUE(type);
    SHOWVALUE(protocol);

    /* The TCP/IP stack may not have been up when this program was started
     * (anything running before the startup-sequence is in that position);
     * open bsdsocket.library now if it wasn't opened at startup. */
    CHECK_SOCKET_LIBRARY_R(__clib4, ERROR);

    /* Since Linux 2.6.27 the type argument may carry SOCK_NONBLOCK, saving the
     * separate fcntl() round trip; it is the idiom most modern code uses.  We
     * passed the whole type straight through to the 4.4BSD stack, which knows
     * only SOCK_STREAM(1)/SOCK_DGRAM(2)/SOCK_RAW(3) -- SOCK_NONBLOCK is
     * O_NONBLOCK, (1<<6), so socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)
     * arrived as type 65 and simply failed.  Strip the bit, remember it, and
     * apply it below once the descriptor exists.
     *
     * SOCK_CLOEXEC is #defined to 0 in <sys/socket.h> here, so it cannot be
     * distinguished yet; giving it a real value is a header change to make
     * separately.
     */
    type_flags = type & SOCK_NONBLOCK;
    type &= ~SOCK_NONBLOCK;

    __stdio_lock(__clib4);

    fd_slot_number = __find_vacant_fd_entry(__clib4);
    if (fd_slot_number < 0) {
        if (__grow_fd_table(__clib4, 0) < 0) {
            SHOWMSG("couldn't find a vacant fd slot and no memory to create one");
            goto out;
        }

        fd_slot_number = __find_vacant_fd_entry(__clib4);
        assert(fd_slot_number >= 0);
    }

    lock = __create_mutex();
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
        type = SOCK_DGRAM;
    }

    socket_fd = __socket(domain, type, protocol);
    if (socket_fd < 0) {
        SHOWMSG("could not create socket");
        goto out;
    }

    fd = __clib4->__fd[fd_slot_number];

    __initialize_fd(fd, __socket_hook_entry, (BPTR) socket_fd, FDF_IN_USE | FDF_IS_SOCKET | FDF_READ | FDF_WRITE, lock);

    lock = NULL;

    /* Apply the SOCK_NONBLOCK stripped above, recording FDF_NON_BLOCKING so
     * the flag and the socket's real state agree -- fcntl(F_SETFL) trusts that
     * flag to decide whether a change is needed at all.
     */
    if (type_flags & SOCK_NONBLOCK) {
        int on = 1;

        if (__IoctlSocket(socket_fd, FIONBIO, &on) < 0) {
            SHOWMSG("could not set non-blocking mode on new socket");
            goto out;
        }
        SET_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
    }

    result = fd_slot_number;

out:

    __stdio_unlock(__clib4);
    __delete_mutex(lock);

    __check_abort_f(__clib4);

    RETURN(result);
    return (result);
}
