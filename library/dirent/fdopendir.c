/*
 * $Id: dirent_fdopendir.c,v 1.0 2022-08-10 19:30:22 clib4devs Exp $
*/

#ifndef _DIRENT_HEADERS_H
#include "dirent_headers.h"
#endif /* _DIRENT_HEADERS_H */

DIR *
fdopendir(int fd) {
    DIR *dir;
    struct stat st;
    struct _clib4 *__clib4 = __CLIB4;

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
    struct fd *fd1 = __get_file_descriptor(__clib4, fd);
    if (fd1 == NULL) {
        __set_errno(EBADF);
        return NULL;
    }

    __fd_lock(fd1);

    dir = opendir(fd1->fd_Aux);
    if (!dir) {
        __set_errno(ENOENT);

        goto out;
    }

    /* Set the fd field on directory handler
     * This will be used on closedir()
     */
    ((struct DirectoryHandle *) dir)->dh_Fd = fd;

out:
    __fd_unlock(fd1);

    return dir;
}