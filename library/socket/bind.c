/*
 * $Id: socket_bind.c,v 1.8 2006-11-16 10:41:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
bind(int sockfd, const struct sockaddr *name, socklen_t namelen) {
    struct fd *fd;
    int result = ERROR;

    ENTER();

    SHOWVALUE(sockfd);
    SHOWPOINTER(name);
    SHOWVALUE(namelen);

    assert(name != NULL);
    assert(__SocketBase != NULL);

    if (name == NULL) {
        SHOWMSG("invalid name parameter");

        __set_errno(EFAULT);
        goto out;
    }

    assert(sockfd >= 0 && sockfd < __num_fd);
    assert(__fd[sockfd] != NULL);
    assert(FLAG_IS_SET(__fd[sockfd]->fd_Flags, FDF_IN_USE));
    assert(FLAG_IS_SET(__fd[sockfd]->fd_Flags, FDF_IS_SOCKET));

    fd = __get_file_descriptor_socket(sockfd);
    if (fd == NULL)
        goto out;


    if (name->sa_family == AF_LOCAL) {
#define un_addr ((struct sockaddr_un *) name)
        struct sockaddr_in sin;
        socklen_t len = namelen - offsetof(struct sockaddr_un, sun_path);
        int file_descriptor;

        /* Check that name is within bounds.  Don't check if the string is
           NUL-terminated, because there are projects out there which set
           namelen to a value which doesn't cover the trailing NUL. */
        if (len <= 1 || (len = strnlen(un_addr->sun_path, len)) > UNIX_PATH_MAX) {
            __set_errno(len <= 1 ? (len == 1 ? ENOENT : EINVAL) : ENAMETOOLONG);
            goto out;
        }
        /* Copy over the sun_path string into a buffer big enough to add a
           trailing NUL. */
        char sun_path[len + 1];
        strncpy(sun_path, un_addr->sun_path, len);
        sun_path[len] = '\0';

        /* This isn't entirely foolproof, but we check first if the file exists
           so we can return with EADDRINUSE before having bound the socket.
           This allows an application to call bind again on the same socket using
           another filename.  If we bind first, the application will not be able
           to call bind successfully ever again. */
        struct ExamineData *fib;

        /* Let's see if we can examine the file. Some file systems
           may not support this. */
        fib = ExamineObjectTags(EX_StringNameInput, sun_path, TAG_DONE);
        if (fib == NULL) {
            if (EXD_IS_FILE(fib)) {
                SHOWMSG("file already exists. Set EADDRINUSE");
                __set_errno(EADDRINUSE);
                FreeDosObject(DOS_EXAMINEDATA, fib);
                goto out;
            }
            FreeDosObject(DOS_EXAMINEDATA, fib);
        }
        else {
            SHOWMSG("file is unusable; we cannot examine the file.");
            goto out;
        }

        sin.sin_family = AF_INET;
        sin.sin_port = 0;
        sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        if (bind(fd->fd_Socket, (struct sockaddr *) &sin, len = sizeof sin)) {
            goto out;
        }
        if (getsockname(fd->fd_Socket, (struct sockaddr *) &sin, &len)) {
            goto out;
        }

        sin.sin_port = ntohs(sin.sin_port);

        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
        if ((file_descriptor = creat(sun_path, mode)) < 0) {
            if (errno == EEXIST) {
                __set_errno(EADDRINUSE);
            }
            goto out;
        } else {
            char buf[sizeof(SOCKET_COOKIE) + 80];
            sprintf(buf, "%s%u %c ", SOCKET_COOKIE, sin.sin_port,
                    get_socket_type(sockfd) == SOCK_STREAM ? 's' : get_socket_type(sockfd) == SOCK_DGRAM ? 'd' : 'r');
            af_local_set_secret(sockfd, strchr(buf, '\0'));
            int status = write(file_descriptor, buf, strlen(buf) + 1);
            if (status > 0) {
                //set_sun_path(sun_path);
                result = 0;
            }
            close(file_descriptor);
            /* Remove write bits */
            SetProtection(sun_path, EXDF_NO_WRITE);
        }
#undef un_addr
    } else {
        result = __bind(fd->fd_Socket, (struct sockaddr *) name, namelen);
    }
    out:

    __check_abort();

    RETURN(result);
    return (result);
}
