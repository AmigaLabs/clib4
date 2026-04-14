/*
 * $Id: mman_mlock.c,v 1.0 2026-04-14 00:00:00 clib4devs Exp $
 *
 * mlock(), munlock(), mlockall(), munlockall() - lock/unlock memory pages
 *
 * POSIX.1-2001 / SVr4.  See man mlock(2).
 *
 * On AmigaOS 4 there is no virtual memory / swap subsystem, so all
 * allocated memory is always physically resident in RAM.  Therefore:
 *
 *   mlock()     / munlock()    - delegate to exec.library LockMem /
 *                                UnlockMem, which pin memory against
 *                                DMA-capable hardware that requires a
 *                                stable physical address.
 *   mlockall()  / munlockall() - no-op success (all memory is always
 *                                resident; no swap exists).
 *
 * LockMem / UnlockMem live directly on struct ExecIFace (IExec):
 *
 *   BOOL LockMem(APTR baseAddress, ULONG size);
 *   void UnlockMem(APTR baseAddress, ULONG size);
 *
 * LockMem returns TRUE on success, FALSE if insufficient memory or the
 * region overlaps an already-locked area that cannot be merged.
 */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/mman.h>
#include <stdint.h>

/* -----------------------------------------------------------------------
 * mlock() - lock a range of pages into physical memory
 *
 * POSIX notes:
 *   - addr need not be page-aligned (Linux rounds down; we accept any
 *     address and pass it directly to LockMem which handles alignment).
 *   - addr + len must not overflow.
 *   - len == 0 is a success no-op.
 *
 * Errors:
 *   EINVAL  addr + len overflows, or len is excessively large.
 *   ENOMEM  Some or all of the range could not be locked (LockMem
 *           returned FALSE).
 * ----------------------------------------------------------------------- */
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

    if (!IExec->LockMem((APTR)(uintptr_t)addr, (ULONG)len)) {
        __set_errno(ENOMEM);
        RETURN(-1);
        return -1;
    }

    RETURN(0);
    return 0;
}

/* -----------------------------------------------------------------------
 * munlock() - unlock a range of pages
 *
 * Errors:
 *   EINVAL  addr + len overflows.
 * ----------------------------------------------------------------------- */
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

    IExec->UnlockMem((APTR)(uintptr_t)addr, (ULONG)len);

    RETURN(0);
    return 0;
}

/* -----------------------------------------------------------------------
 * mlockall() - lock the entire virtual address space of the process
 *
 * On AmigaOS 4 there is no swap, so this is a validated no-op.
 *
 * Errors:
 *   EINVAL  Unknown flags, or MCL_ONFAULT without MCL_CURRENT/MCL_FUTURE.
 * ----------------------------------------------------------------------- */
int
mlockall(int flags)
{
    ENTER();

    SHOWVALUE(flags);

    /* Validate: only known flags */
    if ((unsigned int)flags & ~(unsigned int)(MCL_CURRENT | MCL_FUTURE | MCL_ONFAULT)) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    /* MCL_ONFAULT requires MCL_CURRENT or MCL_FUTURE */
    if ((flags & MCL_ONFAULT) && !(flags & (MCL_CURRENT | MCL_FUTURE))) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    /* AmigaOS 4 has no virtual memory / swap: all memory is always
     * resident, so mlockall() is a successful no-op. */
    RETURN(0);
    return 0;
}

/* -----------------------------------------------------------------------
 * munlockall() - unlock the entire virtual address space of the process
 *
 * On AmigaOS 4 there is no swap, so this is a no-op.
 * ----------------------------------------------------------------------- */
int
munlockall(void)
{
    ENTER();

    /* AmigaOS 4 has no virtual memory / swap: all memory is always
     * resident, so munlockall() is a successful no-op. */
    RETURN(0);
    return 0;
}
