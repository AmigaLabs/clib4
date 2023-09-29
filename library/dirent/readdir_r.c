/*
 * $Id: dirent_readdir_r.c,v 1.10 2022-04-24 10:32:15 clib4devs Exp $
*/

#ifndef _DIRENT_HEADERS_H
#include "dirent_headers.h"
#endif /* _DIRENT_HEADERS_H */

int
readdir_r(DIR *dir, struct dirent *buf, struct dirent **result) {
    struct dirent *de;

    de = readdir(dir);
    if (de)
        memcpy(buf, de, de->d_reclen);
    else
        buf = NULL;

    *result = buf;
    return 0;
}