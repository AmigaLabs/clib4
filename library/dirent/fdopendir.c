/*
 * $Id: dirent_fdopendir.c,v 1.0 2022-08-10 19:30:22 clib2devs Exp $
*/

#ifndef _DIRENT_HEADERS_H
#include "dirent_headers.h"
#endif /* _DIRENT_HEADERS_H */

DIR *
fdopendir(int fd) {
    DIR *dir;
    struct stat st;

    if (fstat(fd, &st) < 0) {
        return 0;
    }
    if (fcntl(fd, F_GETFL) & O_PATH) {
        errno = EBADF;
        return 0;
    }
    if (!S_ISDIR(st.st_mode)) {
        errno = ENOTDIR;
        return 0;
    }
    if (!(dir = calloc(1, sizeof *dir))) {
        return 0;
    }

    fcntl(fd, F_SETFD, FD_CLOEXEC);

    return dir;
}