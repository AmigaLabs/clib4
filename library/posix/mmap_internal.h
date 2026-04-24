/*
 * $Id: mmap_internal.h,v 1.1 2026-04-14 00:00:00 clib4devs Exp $
 *
 * Internal header for mmap tracking - allows msync/munmap/mprotect to
 * manage file-backed and anonymous mappings.
 *
 * Memory layout (page-aligned allocation):
 *
 *   [alloc_base (PAGE_SIZE-aligned)]
 *   ...first PAGE_SIZE bytes...
 *   [mmap_header] <- at (user_ptr - sizeof(mmap_header))
 *   [user_ptr (PAGE_SIZE-aligned)] <- returned to caller
 *   ... length bytes of user data ...
 *
 * This ensures the pointer returned to the user is always page-aligned,
 * which is required by POSIX mmap() and mprotect().
*/

#ifndef _MMAP_INTERNAL_H
#define _MMAP_INTERNAL_H

#include <stdint.h>
#include <sys/types.h>

#define MMAP_MAGIC      0x4D4D4150  /* "MMAP" */
#define MMAP_PAGE_SIZE  4096UL      /* AmigaOS 4 MMU page size */

struct mmap_header {
    uint32_t magic;     /* MMAP_MAGIC for validation */
    void    *alloc_base;/* original memalign()'d / AllocVecTags'd pointer */
    int      fd;        /* dup'd fd for write-back, or -1 for anonymous */
    off_t    offset;    /* file offset this mapping starts at */
    size_t   length;    /* length of the mapping (user data) */
    int      flags;     /* MAP_SHARED, MAP_PRIVATE, etc. */
    int      prot;      /* current PROT_READ/PROT_WRITE/PROT_EXEC/PROT_NONE */
    int      exec_alloc;/* non-zero: alloc_base from AllocVecTags(MEMF_EXECUTABLE) */
};

/*
 * Per-mapping tracking record — held in the global singly-linked list
 * __mmap_records, which is the authoritative source of truth for munmap().
 *
 * Using a tracked list side-steps all magic-value false-positive hazards:
 * file content at (ptr - sizeof_header) cannot spoof our internal state.
 */
struct mmap_record {
    void               *user_ptr;   /* pointer returned to the caller     */
    void               *alloc_base; /* original memalign / AllocVecTags   */
    int                 exec_alloc; /* non-zero → free with FreeVec()     */
    int                 fd;         /* dup()'d file descriptor, or -1     */
    struct mmap_record *next;
};

/* Process-global list of active mmap allocations (protected by the clib4
 * memory mutex via __memory_lock / __memory_unlock).
 * Defined in mmap.c; declared here for munmap.c / mprotect.c / msync.c.  */
extern struct mmap_record * volatile __mmap_records;

/* Get the in-page header for a user pointer produced by our mmap().
 * Used only for read-only metadata access (mprotect, msync, writeback).
 * munmap() MUST use __mmap_records for freeing — never this function.
 *
 * Returns NULL if the pointer does not satisfy all three of:
 *   1. magic == MMAP_MAGIC
 *   2. alloc_base != NULL
 *   3. alloc_base + MMAP_PAGE_SIZE == user_ptr  (structural layout invariant)
 */
static inline struct mmap_header *
__mmap_get_header(void *user_ptr) {
    struct mmap_header *hdr = (struct mmap_header *)((char *)user_ptr - sizeof(struct mmap_header));
    if (hdr->magic == MMAP_MAGIC &&
        hdr->alloc_base != NULL &&
        (char *)hdr->alloc_base + MMAP_PAGE_SIZE == (char *)user_ptr)
        return hdr;
    return NULL;
}

/* Get the user pointer from a header */
static inline void *
__mmap_get_user_ptr(struct mmap_header *hdr) {
    return (void *)((char *)hdr + sizeof(struct mmap_header));
}

/* Write back a MAP_SHARED file-backed mapping to its file */
static inline int
__mmap_writeback(struct mmap_header *hdr, void *user_ptr, size_t len) {
    if (hdr == NULL)
        return -1;
    if (!(hdr->flags & MAP_SHARED))
        return 0; /* MAP_PRIVATE: no write-back needed */
    if (hdr->fd < 0)
        return 0; /* anonymous: no file to write to */
    if (!(hdr->prot & PROT_WRITE))
        return 0; /* read-only mapping: nothing to write back */

    /* Clamp len to the actual mapping length */
    if (len > hdr->length)
        len = hdr->length;

    off_t saved = lseek(hdr->fd, 0, SEEK_CUR);
    lseek(hdr->fd, hdr->offset, SEEK_SET);
    ssize_t written = write(hdr->fd, user_ptr, len);
    lseek(hdr->fd, saved, SEEK_SET);

    return (written == (ssize_t)len) ? 0 : -1;
}

#endif /* _MMAP_INTERNAL_H */
