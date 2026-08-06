/*
 * $Id: socket_accept.c,v 1.17 2006-11-16 10:41:15 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include <sys/ioctl.h>

int
accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen) {
    APTR lock = NULL;
    struct fd *fd = NULL;
    struct fd *new_fd;
    int new_fd_slot_number;
    int result = ERROR;
    LONG socket_fd;
    LONG new_socket_fd = -1;
    BOOL stdio_locked = FALSE;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(sockfd);
    SHOWPOINTER(cliaddr);
    SHOWPOINTER(addrlen);

    DECLARE_SOCKETBASE_R(__clib4);

    assert(sockfd >= 0 && sockfd < __clib4->__num_fd);
    assert(__clib4->__fd[sockfd] != NULL);
    assert(FLAG_IS_SET(__clib4->__fd[sockfd]->fd_Flags, FDF_IN_USE));
    assert(FLAG_IS_SET(__clib4->__fd[sockfd]->fd_Flags, FDF_IS_SOCKET));

    /* We need to know which parameter to submit to the accept()
       call first. */
    __stdio_lock(__clib4);
    stdio_locked = TRUE;

    fd = __get_file_descriptor_socket(__clib4, sockfd);
    if (fd == NULL)
        goto out;

    /* Remember the socket number for later. */
    socket_fd = fd->fd_Socket;

    /* Now let go of the stdio lock, so that the only locking performed
       will be done inside the accept() call. */
    __stdio_unlock(__clib4);
    stdio_locked = FALSE;

    /* Wait for the accept() to complete, then hook up the socket
       with a file descriptor. */
    new_socket_fd = __accept(socket_fd, cliaddr, addrlen);
    if (new_socket_fd < 0) {
        SHOWMSG("could not accept connection");
        goto out;
    }

    /* Linux does NOT let the accepted socket inherit the listening socket's
     * file status flags -- accept(2) says so explicitly: "the new socket
     * returned by accept() does not inherit file status flags such as
     * O_NONBLOCK and O_ASYNC from the listening socket".  The 4.4BSD stack
     * underneath us does inherit them, so clear it here to match Linux.
     *
     * This is not merely cosmetic.  __initialize_fd() below always registers
     * the new descriptor without FDF_NON_BLOCKING, and fcntl(F_SETFL) only
     * issues file_action_set_blocking when the requested mode differs from
     * that flag.  So an inherited non-blocking socket was unfixable from the
     * outside: fcntl saw "already blocking, nothing to do" while the socket
     * really was non-blocking, and the first read() came back EAGAIN.  That is
     * exactly the sequence OpenJDK's java.net performs -- it deliberately puts
     * listening sockets into non-blocking mode and relies on accept() handing
     * back a blocking one -- and it made every accepted connection fail with
     * "Resource temporarily unavailable".
     *
     * Forcing the mode keeps the descriptor's real state and the FDF_ flags
     * consistent, which is what makes fcntl() usable on it afterwards.
     */
    {
        int off = 0;

        if (__IoctlSocket(new_socket_fd, FIONBIO, &off) < 0)
            SHOWMSG("could not clear non-blocking mode on accepted socket");
    }

    /* OK, back to work: we'll need to manipulate the file
       descriptor tables. */
    __stdio_lock(__clib4);
    stdio_locked = TRUE;

    new_fd_slot_number = __find_vacant_fd_entry(__clib4);
    if (new_fd_slot_number < 0) {
        if (__grow_fd_table(__clib4, 0) < 0) {
            SHOWMSG("couldn't find a vacant fd slot and no memory to create one");
            goto out;
        }

        new_fd_slot_number = __find_vacant_fd_entry(__clib4);
        assert(new_fd_slot_number >= 0);
    }
    lock = __create_mutex();
    if (lock == NULL) {
        errno = ENOMEM;
        goto out;
    }

    new_fd = __clib4->__fd[new_fd_slot_number];

    __initialize_fd(new_fd, __socket_hook_entry, (BPTR) new_socket_fd,
                    FDF_IN_USE | FDF_IS_SOCKET | FDF_READ | FDF_WRITE, lock);

    lock = NULL;

    result = new_fd_slot_number;

    new_socket_fd = -1;

out:

    if (new_socket_fd != -1) {
        __CloseSocket(new_socket_fd);
    }

    if (stdio_locked)
        __stdio_unlock(__clib4);

    __delete_mutex(lock);

    __check_abort_f(__clib4);

    RETURN(result);
    return (result);
}
