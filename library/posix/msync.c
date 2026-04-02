/*
 * $Id: mman_mmap.c,v 1.0 2021-01-18 20:17:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/mman.h>
#include "mmap_internal.h"

int msync(void *addr, size_t len, int flags) {
    (void)(flags);

    ENTER();

    SHOWPOINTER(addr);
    SHOWVALUE(len);
    SHOWVALUE(flags);

    if (addr == NULL) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    struct mmap_header *hdr = __mmap_get_header(addr);
    if (hdr) {
        int ret = __mmap_writeback(hdr, addr, len);
        RETURN(ret);
        return ret;
    }

    /* No tracking header found - could be a legacy mapping or plain memory.
     * Return success for backward compatibility. */
    RETURN(0);
    return 0;
}