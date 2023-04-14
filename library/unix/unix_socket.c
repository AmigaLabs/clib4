/*
 * $Id: unix_unix_socket.c,v 1.0 2023-04-14 10:41:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include "unix_socket.h"

int
get_socket_type(int sockfd) {
    struct fd *fd = __get_file_descriptor_socket(sockfd);
    return  fd->fd_Type;
}

int
get_addr_family(int sockfd) {
    struct fd *fd = __get_file_descriptor_socket(sockfd);
    return  fd->fd_Family;
}

int
get_addr_protocol(int sockfd) {
    struct fd *fd = __get_file_descriptor_socket(sockfd);
    return  fd->fd_Protocol;
}

void
af_local_set_cred(int sockfd) {
    struct fd *fd = __get_file_descriptor_socket(sockfd);
    // DO NOTHING AT MOMENT SINCE WE DON'T HAVE USERS
}

void
af_local_set_secret(int sockfd, char *buf) {
    struct fd *fd = __get_file_descriptor_socket(sockfd);
    fd->fd_Secret[0] = arc4random();
    fd->fd_Secret[1] = arc4random();
    fd->fd_Secret[2] = arc4random();
    fd->fd_Secret[3] = arc4random();
    sprintf(buf, "%08x-%08x-%08x-%08x", fd->fd_Secret[0], fd->fd_Secret[1], fd->fd_Secret[2], fd->fd_Secret[3]);
}

void
set_peer_sun_path(int sockfd, const char *path) {
    struct fd *fd = __get_file_descriptor_socket(sockfd);
    fd->fd_UserData = path ? strdup (path) : NULL;
}

const char *
get_peer_sun_path(int sockfd) {
    struct fd *fd = __get_file_descriptor_socket(sockfd);
    return (const char *) fd->fd_UserData;
}

int
get_inet_addr(const struct sockaddr *in, int inlen, struct sockaddr_storage *out, socklen_t *outlen, int *type, int *secret) {
    int secret_buf[4];
    int *secret_ptr = (secret ?: secret_buf);

    switch (in->sa_family) {
        case AF_LOCAL:
            /* Check for abstract socket. These are generated for AF_LOCAL datagram
               sockets in recv_internal, to allow a datagram server to use sendto after recvfrom. */
            if (inlen >= (int) sizeof(in->sa_family) + 7
                && in->sa_data[0] == '\0' && in->sa_data[1] == 'd'
                && in->sa_data[6] == '\0') {
                struct sockaddr_in addr;
                addr.sin_family = AF_INET;
                sscanf((const char *) in->sa_data + 2, "%04hx", &addr.sin_port);
                addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
                *outlen = sizeof addr;
                memcpy(out, &addr, *outlen);
                return 0;
            }
            break;
        case AF_INET:
            memcpy(out, in, inlen);
            *outlen = inlen;
            /* If the peer address given in connect or sendto is the ANY address,
               Winsock fails with WSAEADDRNOTAVAIL, while Linux converts that into
               a connection/send attempt to LOOPBACK.  We're doing the same here. */
            if (((struct sockaddr_in *) out)->sin_addr.s_addr == htonl(INADDR_ANY))
                ((struct sockaddr_in *) out)->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            return 0;
        case AF_INET6:
            memcpy(out, in, inlen);
            *outlen = inlen;
            /* See comment in AF_INET case. */
            if (IN6_IS_ADDR_UNSPECIFIED(&((struct sockaddr_in6 *) out)->sin6_addr))
                ((struct sockaddr_in6 *) out)->sin6_addr = in6addr_loopback;
            return 0;
        default:
            __set_errno(EAFNOSUPPORT);
            return ERROR;
    }
    /* AF_LOCAL/AF_UNIX only */
    int fd = open((const char *) in->sa_data, O_RDONLY, 0);
    if (fd < 0) {
        return ERROR;
    }

    /* Now test for the bits. */
    struct ExamineData *fib;

    /* Let's see if we can examine the file. Some file systems
       may not support this. */
    fib = ExamineObjectTags(EX_StringNameInput, in->sa_data, TAG_DONE);
    if (fib == NULL) {
        close(fd);
        SHOWMSG("file is unusable; we cannot examine the file.");
        return ERROR;
    }

    if (FLAG_IS_CLEAR(fib->Protection, EXDF_NO_WRITE)) {
        FreeDosObject(DOS_EXAMINEDATA, fib);
        close(fd);
        SHOWMSG("this object is not write enabled");
        __set_errno(EACCES);
        return ERROR;
    }
    FreeDosObject(DOS_EXAMINEDATA, fib);

    /* Eventually check the content and fetch the required information. */
    char buf[128];
    memset(buf, 0, sizeof buf);
    int status = read(fd, buf, 128);
    close(fd);
    if (status > 0) {
        struct sockaddr_in sin;
        char ctype;
        sin.sin_family = AF_INET;
        if (strncmp(buf, SOCKET_COOKIE, strlen(SOCKET_COOKIE))) {
            __set_errno(EBADF);
            return ERROR;
        }
        sscanf(buf + strlen(SOCKET_COOKIE), "%hu %c %08x-%08x-%08x-%08x",
               &sin.sin_port,
               &ctype,
               secret_ptr, secret_ptr + 1, secret_ptr + 2, secret_ptr + 3);
        sin.sin_port = htons(sin.sin_port);
        sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        memcpy(out, &sin, sizeof sin);
        *outlen = sizeof sin;
        if (type)
            *type = (ctype == 's' ? SOCK_STREAM :
                     ctype == 'd' ? SOCK_DGRAM
                                  : SOCK_RAW);
        return 0;
    }

    return ERROR;
}