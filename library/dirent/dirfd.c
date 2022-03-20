/*
 * $Id: dirent_dirfd.c,v 1.0 2021-02-21 19:30:22 clib2devs Exp $
*/

#ifndef _DIRENT_HEADERS_H
#include "dirent_headers.h"
#endif /* _DIRENT_HEADERS_H */

int 
dirfd(DIR *dirp) {
    (void)(dirp);

    __set_errno(ENOTSUP);

    return -1;
}