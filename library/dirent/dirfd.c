/*
 * $Id: dirent_dirfd.c,v 1.1 2022-08-12 19:30:22 clib2devs Exp $
*/

#ifndef _DIRENT_HEADERS_H
#include "dirent_headers.h"
#endif /* _DIRENT_HEADERS_H */

int 
dirfd(DIR *dirp) {

    if (dirp == NULL) {
        __set_errno(EBADF);
        return -1;
    }

    return ((struct DirectoryHandle *) dirp)->dh_Fd;
}