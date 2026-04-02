/*
 * $Id: mman_mmap.c,v 1.0 2021-01-18 20:17:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/mman.h>
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

    /* Allocate memory with space for our tracking header */
    void *block = calloc(1, sizeof(struct mmap_header) + len);
    if (!block) {
        __set_errno(ENOMEM);
        RETURN(MAP_FAILED);
        return MAP_FAILED;
    }

    struct mmap_header *hdr = (struct mmap_header *)block;
    void *user_ptr = __mmap_get_user_ptr(hdr);

    /* Fill in the tracking header */
    hdr->magic = MMAP_MAGIC;
    hdr->offset = offset;
    hdr->length = len;
    hdr->flags = flags;
    hdr->prot = prot;
    hdr->fd = -1;

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

    RETURN(user_ptr);
    return user_ptr;
}