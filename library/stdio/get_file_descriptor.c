/*
 * $Id: stdio_get_file_descriptor.c,v 1.6 2023-07-04 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

enum resolution_mode_t {
    resolution_mode_exact,    /* Return the descriptor exactly as it is. */
    resolution_mode_alias    /* Resolve the descriptor's alias, if necessary. */
};

static struct fd *
get_file_descriptor(struct _clib4 *__clib4, int file_descriptor, enum resolution_mode_t resolution_mode) {
    struct fd *result = NULL;
    struct fd *fd;

    __stdio_lock(__clib4);

    if (file_descriptor < 0 || file_descriptor >= __clib4->__num_fd) {
        SHOWMSG("invalid file descriptor");
        goto out;
    }

    fd = __clib4->__fd[file_descriptor];
    if (fd == NULL) {
        SHOWMSG("invalid file descriptor");
        goto out;
    }

    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE)) {
        SHOWMSG("this descriptor is not even in use");
        goto out;
    }

    /* Move up to the original file, if this is an alias. */
    if (resolution_mode == resolution_mode_alias && fd->fd_Original != NULL)
        fd = fd->fd_Original;

    result = fd;

out:

    __stdio_unlock(__clib4);

    return (result);
}

struct fd *
__get_file_descriptor(struct _clib4 *__clib4, int file_descriptor) {
    struct fd *result;

    result = get_file_descriptor(__clib4, file_descriptor, resolution_mode_alias);

    return (result);
}

struct fd *
__get_file_descriptor_dont_resolve(struct _clib4 *__clib4, int file_descriptor) {
    struct fd *result;

    result = get_file_descriptor(__clib4, file_descriptor, resolution_mode_exact);

    return (result);
}
