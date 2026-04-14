/*
 * $Id: mman_munlock.c,v 1.0 2026-04-14 00:00:00 clib4devs Exp $
 *
 * munlock() - unlock a range of pages
 *
 * POSIX.1-2001 / SVr4.  See man mlock(2).
 *
 * On AmigaOS 4, UnlockMem lives directly on struct ExecIFace (IExec):
 *   void UnlockMem(APTR baseAddress, ULONG size);
 *
 * Errors:
 *   EINVAL  addr + len overflows.
 */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/mman.h>
#include <stdint.h>

int
munlock(const void *addr, size_t len)
{
    ENTER();

    SHOWPOINTER(addr);
    SHOWVALUE(len);

    if (len == 0) {
        RETURN(0);
        return 0;
    }

    /* Detect address-range overflow */
    if ((uintptr_t)addr + len < (uintptr_t)addr) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    UnlockMem((APTR)(uintptr_t)addr, (ULONG)len);

    RETURN(0);
    return 0;
}
