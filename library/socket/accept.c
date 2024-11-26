/*
 * $Id: socket_accept.c,v 1.17 2006-11-16 10:41:15 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen) {
    struct SignalSemaphore *lock = NULL;
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

    DECLARE_SOCKETBASE();

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
    lock = __create_semaphore();
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

    __delete_semaphore(lock);

    __check_abort_f(__clib4);

    RETURN(result);
    return (result);
}
