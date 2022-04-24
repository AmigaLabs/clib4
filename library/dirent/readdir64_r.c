/*
 * $Id: dirent_readdir64_r.c,v 1.10 2022-04-24 10:32:15 clib2devs Exp $
*/

#ifndef _DIRENT_HEADERS_H
#include "dirent_headers.h"
#endif /* _DIRENT_HEADERS_H */

int
readdir64_r(DIR *dir, struct dirent *buf, struct dirent **result) {
    return readdir_r(dir, buf, result);
}