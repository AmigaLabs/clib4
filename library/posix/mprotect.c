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

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif

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

    /* Update mmap tracking header if this is a managed mmap region.
     *
     * CRITICAL: __mmap_get_header(addr) reads from (addr - sizeof(header)).
     * If addr is an interior page of a large JIT allocation (e.g. SpiderMonkey's
     * 128 MiB MEMF_EXECUTABLE pool), and the PRECEDING 64 KiB JIT page was
     * decommitted (MEMATTRF_SUPER_RW), that read causes a DSI in user mode —
     * which on AmigaOS silently suspends the task (no crashlog), manifesting as
     * a hang at the splashscreen.
     *
     * Safe approach: first check the tracking list to confirm addr is a known
     * mmap() root, without reading any bytes before addr.  Only if found do we
     * access the in-page header (which is guaranteed to be in a valid page).
     */
    {
        struct mmap_record *rec = NULL;
        struct _clib4 *__clib4 = __CLIB4;
        __memory_lock(__clib4);
        for (struct mmap_record *r = __mmap_records; r != NULL; r = r->next) {
            if (r->user_ptr == addr) { rec = r; break; }
        }
        __memory_unlock(__clib4);

        if (rec != NULL) {
            /*
             * Do NOT grant PROT_EXEC on memory that was not allocated from
             * the MEMF_EXECUTABLE pool: SetMemoryAttrs(MEMATTRF_EXECUTE) on
             * non-executable physical memory is ignored by the E5500 I-TLB and
             * would result in an ISI at execution time.
             */
            if ((prot & PROT_EXEC) && !rec->exec_alloc) {
                __set_errno(EACCES);
                RETURN(-1);
                return -1;
            }
            /* Safe to access the header now (addr is a confirmed mmap root) */
            struct mmap_header *hdr = __mmap_get_header(addr);
            if (hdr != NULL)
                hdr->prot = prot;
        }
    }

    /*
     * Apply MMU protection via exec.library's MMU interface.
     *
     * __IMMU is cached once at libInit time (clib4.c) to avoid the
     * heavy GetInterface / DropInterface overhead on every call.
     * When the JIT calls ReprotectRegion hundreds of times per second,
     * GetInterface/DropInterface on each call serializes on a semaphore
     * inside exec.library and causes the visible CPU-at-100% spin.
     *
     * Notes:
     *  - GetMemoryAttrs preserves cache/coherency flags when we merge bits.
     *  - SetMemoryAttrs silently ignores ranges that contain unmapped pages.
     *  - Both functions must be called in supervisor mode.
     *  - UserState() must only be called when SuperState() returned non-NULL.
     */
    struct MMUIFace *IMMU = __IMMU;  /* use cached interface */

    if (IMMU != NULL) {
        APTR stack = SuperState();
        ULONG current = GetMemoryAttrs(addr, 0);
        SetMemoryAttrs(addr, (ULONG)len, __prot_to_memattrf(prot, current));
        if (stack != NULL) {
            UserState(stack);
        }
    }

    RETURN(0);
    return 0;
}
