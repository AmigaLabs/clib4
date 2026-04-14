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
    void    *alloc_base;/* original memalign()'d pointer to free */
    int      fd;        /* dup'd fd for write-back, or -1 for anonymous */
    off_t    offset;    /* file offset this mapping starts at */
    size_t   length;    /* length of the mapping (user data) */
    int      flags;     /* MAP_SHARED, MAP_PRIVATE, etc. */
    int      prot;      /* current PROT_READ/PROT_WRITE/PROT_EXEC/PROT_NONE */
};

/* Get the header from a user-facing mmap pointer.
 * Returns NULL if the pointer was not produced by our mmap(). */
static inline struct mmap_header *
__mmap_get_header(void *user_ptr) {
    struct mmap_header *hdr = (struct mmap_header *)((char *)user_ptr - sizeof(struct mmap_header));
    if (hdr->magic == MMAP_MAGIC)
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
