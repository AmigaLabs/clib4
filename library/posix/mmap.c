/*
 * $Id: mman_mmap.c,v 1.1 2026-04-14 00:00:00 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/mman.h>
#include <malloc.h>
#include "mmap_internal.h"

void *
mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset) {
    ENTER();

    SHOWPOINTER(addr);
    SHOWVALUE(len);
    SHOWVALUE(prot);
    SHOWVALUE(flags);
    SHOWVALUE(fd);
    SHOWVALUE(offset);

    if (len == 0) {
        __set_errno(EINVAL);
        errno = EINVAL;
        RETURN(MAP_FAILED);
        return MAP_FAILED;
    }

    /*
     * Allocate a page-aligned block with space for our tracking header.
     * Layout:
     *   [alloc_base ... alloc_base+PAGE_SIZE-1] <- header page (header sits
     *                                              at end of this page)
     *   [user_ptr ... user_ptr+len-1]           <- user data (page-aligned)
     *
     * This ensures the returned pointer satisfies POSIX page-alignment
     * requirements and that mprotect() can be safely applied to it.
     */
    size_t alloc_size = MMAP_PAGE_SIZE + len;
    void *block = memalign(MMAP_PAGE_SIZE, alloc_size);
    if (block == NULL) {
        __set_errno(ENOMEM);
        RETURN(MAP_FAILED);
        return MAP_FAILED;
    }
    memset(block, 0, alloc_size);

    /* Place header just before the page-aligned user pointer */
    void *user_ptr = (char *)block + MMAP_PAGE_SIZE;
    struct mmap_header *hdr = (struct mmap_header *)((char *)user_ptr - sizeof(struct mmap_header));

    /* Fill in the tracking header */
    hdr->magic      = MMAP_MAGIC;
    hdr->alloc_base = block;
    hdr->offset     = offset;
    hdr->length     = len;
    hdr->flags      = flags;
    hdr->prot       = prot;
    hdr->fd         = -1;

    if (fd >= 0) {
        /* File-backed mapping: dup the fd so caller can close theirs */
        int dfd = dup(fd);
        if (dfd < 0) {
            free(block);
            __set_errno(ENOMEM);
            RETURN(MAP_FAILED);
            return MAP_FAILED;
        }
        hdr->fd = dfd;

        /* Read file content into the mapping */
        lseek(dfd, offset, SEEK_SET);
        read(dfd, user_ptr, len);
    }

    (void)addr; /* MAP_FIXED not yet supported; addr hint is ignored */

    RETURN(user_ptr);
    return user_ptr;
}