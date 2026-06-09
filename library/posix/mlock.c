/*
 * $Id: mman_mlock.c,v 1.0 2026-04-14 00:00:00 clib4devs Exp $
 *
 * mlock() - lock a range of pages into physical memory
 *
 * POSIX.1-2001 / SVr4.  See man mlock(2).
 *
 * On AmigaOS 4, LockMem lives directly on struct ExecIFace (IExec):
 *   BOOL LockMem(APTR baseAddress, ULONG size);
 *
 * LockMem returns TRUE on success, FALSE if insufficient memory or the
 * region overlaps an already-locked area that cannot be merged.
 *
 * POSIX notes:
 *   - addr need not be page-aligned (Linux rounds down; we accept any
 *     address and pass it directly to LockMem which handles alignment).
 *   - addr + len must not overflow.
 *   - len == 0 is a success no-op.
 *
 * Errors:
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
mlock(const void *addr, size_t len)
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

    if (!LockMem((APTR)(uintptr_t)addr, (ULONG)len)) {
        __set_errno(ENOMEM);
        RETURN(-1);
        return -1;
    }

    RETURN(0);
    return 0;
}

