/*
 * $Id: mman_munmap.c,v 1.0 2021-01-18 20:17:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/mman.h>

int
munmap(void *map, size_t length) {
    (void) (length);

    if (map != NULL)
        free(map);

    return 0;
}