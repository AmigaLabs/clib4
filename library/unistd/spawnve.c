/*
 * $Id: unistd_spawnve.c,v 1.0 2024-08-18 18:26:47 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int spawnve(int mode, const char *path, const char **argv, char * const envp[]) {
    return spawnv(mode, path, argv);
}
