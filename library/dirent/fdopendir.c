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
        return NULL;
    }
    if (fcntl(fd, F_GETFL) & O_PATH) {
        __set_errno(EBADF);
        return NULL;
    }
    if (!S_ISDIR(st.st_mode)) {
        __set_errno(ENOTDIR);
        return NULL;
    }

    dir = opendir(st.st_name);
    if (!dir) {
        __set_errno(ENOENT);
        return NULL;
    }

    return dir;
}