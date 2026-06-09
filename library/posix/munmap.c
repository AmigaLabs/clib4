/*
 * $Id: mman_munmap.c,v 1.1 2026-04-15 00:00:00 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif

#include <sys/mman.h>
#include <proto/exec.h>
#include "mmap_internal.h"

/*
 * Remove and return the tracking record for user_ptr from the global list.
 * Caller must NOT hold the memory mutex when calling this function.
 * It acquires and releases the mutex internally.
 * Returns NULL if no record is found (not one of our mappings).
 */
static struct mmap_record *
__mmap_record_take(void *user_ptr)
{
    struct mmap_record * volatile *pp;
    struct mmap_record *rec;
    struct _clib4 *__clib4 = __CLIB4;

    __memory_lock(__clib4);
    pp = &__mmap_records;
    while (*pp && (*pp)->user_ptr != user_ptr)
        pp = &(*pp)->next;
    rec = *pp;
    if (rec)
        *pp = rec->next;
    __memory_unlock(__clib4);

    return rec;
}

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

    /*
     * Look up the pointer in our authoritative tracking list first.
     * This is safe even if the in-page header has been corrupted or
     * is a false positive (file content matching MMAP_MAGIC).
     */
    struct mmap_record *rec = __mmap_record_take(map);
    if (rec) {
        /*
         * Still try to use the in-page header for MAP_SHARED write-back
         * (read-only access — no risk of acting on a corrupted alloc_base).
         */
        struct mmap_header *hdr = __mmap_get_header(map);
        if (hdr != NULL) {
            __mmap_writeback(hdr, map, hdr->length);
            hdr->magic = 0; /* Invalidate to catch double-munmap early */
        }

        /* Close the dup()'d file descriptor stored in the record */
        if (rec->fd >= 0)
            close(rec->fd);

        /* Free the backing allocation — always via FreeVec() since mmap()
         * now allocates all backing blocks (both MEMF_SHARED and
         * MEMF_EXECUTABLE) with AllocVecTags() directly. */
        FreeVec(rec->alloc_base);

        /* Release the tracking record itself */
        FreeVec(rec);

        RETURN(0);
        return 0;
    }

    /*
     * Not in our tracking list — not a mapping we allocated.
     * Do NOT call free(map): the pointer is not from our allocator and
     * calling free() on it would corrupt the heap.
     * Return success silently (the pointer was already unmapped or was
     * never from mmap()).
     */
    RETURN(0);
    return 0;
}