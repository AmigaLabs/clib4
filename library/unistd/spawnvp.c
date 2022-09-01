/*
 * $Id: unistd_spawnvp.c,v 1.0 2021-01-21 19:58:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
spawnvp(int mode, const char *path, const char **argv) {
    return spawnv(mode, path, argv);
}