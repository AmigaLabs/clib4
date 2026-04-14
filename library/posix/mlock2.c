/*
 * $Id: mman_mlock2.c,v 1.0 2026-04-14 00:00:00 clib4devs Exp $
 *
 * mlock2() - lock a range of pages with extended flags
 *
 * Linux-specific.  See man mlock(2).
 *
 * Flags:
 *   0             - behaves exactly like mlock().
 *   MLOCK_ONFAULT - lock currently resident pages; mark non-resident
 *                   pages to be locked when they are faulted in.
 *
 * On AmigaOS 4 there is no virtual memory / swap subsystem and therefore
 * no concept of non-resident pages.  Both flag values are treated
 * identically: delegate to IExec->LockMem(), which is the same behaviour
 * as mlock().
 *
 * Errors:
 *   EINVAL  Unknown flags were specified.
 *   EINVAL  addr + len overflows.
 *   ENOMEM  Some or all of the range could not be locked (LockMem
 *           returned FALSE).
 */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/mman.h>
#include <stdint.h>

int
mlock2(const void *addr, size_t len, unsigned int flags)
{
    ENTER();

    SHOWPOINTER(addr);
    SHOWVALUE(len);
    SHOWVALUE(flags);

    /* Only MLOCK_ONFAULT is a known flag; reject everything else. */
    if (flags & ~(unsigned int)MLOCK_ONFAULT) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

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

    /*
     * AmigaOS 4 has no swap: all memory is always physically resident.
     * MLOCK_ONFAULT therefore has no distinct meaning from 0 — we simply
     * delegate to LockMem() in both cases.
     */
    if (!LockMem((APTR)(uintptr_t)addr, (ULONG)len)) {
        __set_errno(ENOMEM);
        RETURN(-1);
        return -1;
    }

    RETURN(0);
    return 0;
}
