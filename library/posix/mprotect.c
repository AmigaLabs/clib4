/*
 * $Id: mman_mprotect.c,v 1.0 2026-04-14 00:00:00 clib4devs Exp $
 *
 * mprotect() - set protection on a region of memory
 *
 * POSIX.1-2001 / SVr4.  See man mprotect(2).
 *
 * On AmigaOS 4 the MMU protection is enforced via exec.library's
 * SetMemoryAttrs(), which must be called in supervisor mode.  POSIX
 * protection flags are mapped to AmigaOS memory attribute flags:
 *
 *   PROT_NONE              -> MEMATTRF_SUPER_RW     (user: no access)
 *   PROT_READ (no write)   -> MEMATTRF_READ_ONLY    (user: read-only)
 *   PROT_WRITE / READ|WRITE-> MEMATTRF_READ_WRITE   (user: RW)
 *   PROT_EXEC              -> additionally set MEMATTRF_EXECUTE
 *
 * Non-protection MMU attributes (cache, coherency, etc.) are preserved
 * by reading the current attributes first and merging in the new bits.
 *
 * When the target pointer was returned by mmap(), the prot field of its
 * tracking header is also updated so that msync() / munmap() can make
 * informed decisions.
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/mman.h>
#include <exec/memory.h>
#include <interfaces/exec.h>
#include <stdint.h>
#include "mmap_internal.h"

/*
 * Map POSIX prot flags to AmigaOS MMU memory-attribute bits, preserving
 * the non-protection bits from current_attrs (cache, coherency, etc.).
 */
static ULONG
__prot_to_memattrf(int prot, ULONG current_attrs)
{
    /* Strip only the protection-related bits; keep cache/coherency flags */
    ULONG attrs = current_attrs & ~((ULONG)(MEMATTRF_RW_MASK | MEMATTRF_EXECUTE));

    if (prot == PROT_NONE) {
        /* User mode: no access; supervisor: R/W */
        attrs |= MEMATTRF_SUPER_RW;
    } else if (prot & PROT_WRITE) {
        /* User mode: read/write */
        attrs |= MEMATTRF_READ_WRITE;
    } else {
        /* PROT_READ and/or PROT_EXEC without PROT_WRITE: user read-only */
        attrs |= MEMATTRF_READ_ONLY;
    }

    if (prot & PROT_EXEC) {
        attrs |= MEMATTRF_EXECUTE;
    }

    return attrs;
}

int
mprotect(void *addr, size_t len, int prot)
{
    ENTER();

    SHOWPOINTER(addr);
    SHOWVALUE(len);
    SHOWVALUE(prot);

    /* addr must not be NULL */
    if (addr == NULL) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    /* A zero-length range is a no-op */
    if (len == 0) {
        RETURN(0);
        return 0;
    }

    /* addr must be page-aligned (POSIX requirement) */
    if ((uintptr_t)addr & (MMAP_PAGE_SIZE - 1)) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    /* Only valid POSIX prot flags are accepted */
    if ((unsigned int)prot & ~(unsigned int)(PROT_READ | PROT_WRITE | PROT_EXEC | PROT_SEM)) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    /* Update mmap tracking header if this is a managed mmap region */
    struct mmap_header *hdr = __mmap_get_header(addr);
    if (hdr != NULL) {
        hdr->prot = prot;
    }

    /*
     * Apply MMU protection via exec.library's MMU interface.
     *
     * GetMemoryAttrs / SetMemoryAttrs live in the "mmu" interface of
     * exec.library (struct MMUIFace), not in IExec.  We obtain it on
     * demand and release it immediately after use.
     *
     * Notes:
     *  - GetMemoryAttrs preserves cache/coherency flags when we merge bits.
     *  - SetMemoryAttrs silently ignores ranges that contain unmapped pages.
     *  - Both functions must be called in supervisor mode.
     *  - UserState() must only be called when SuperState() returned non-NULL.
     */
    struct MMUIFace *IMMU = (struct MMUIFace *)
        GetInterface((struct Library *)IExec->Data.LibBase, "mmu", 1, NULL);

    if (IMMU != NULL) {
        APTR stack = SuperState();
        ULONG current = GetMemoryAttrs(addr, 0);
        SetMemoryAttrs(addr, (ULONG)len, __prot_to_memattrf(prot, current));
        if (stack != NULL) {
            UserState(stack);
        }
        DropInterface((struct Interface *)IMMU);
    }

    RETURN(0);
    return 0;
}
