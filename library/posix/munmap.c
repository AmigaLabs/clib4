/*
 * $Id: mman_munmap.c,v 1.0 2021-01-18 20:17:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/mman.h>
#include "mmap_internal.h"

int
munmap(void *map, size_t length) {
    ENTER();

    SHOWPOINTER(map);
    SHOWVALUE(length);

    if (map == NULL || map == MAP_FAILED) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    struct mmap_header *hdr = __mmap_get_header(map);
    if (hdr) {
        /* Write back MAP_SHARED file-backed mappings before freeing */
        __mmap_writeback(hdr, map, hdr->length);

        /* Close our dup'd fd */
        if (hdr->fd >= 0) {
            close(hdr->fd);
            hdr->fd = -1;
        }

        hdr->magic = 0; /* Invalidate */
        free(hdr);       /* Free the whole block (header + data) */
    } else {
        /* Legacy path: no tracking header, just free as before */
        free(map);
    }

    RETURN(0);
    return 0;
}