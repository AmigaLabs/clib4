/*
 * $Id: stdio_remove_fd_alias.c,v 1.5 2006-10-10 13:39:26 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
__remove_fd_alias(struct _clib4 *__clib4, struct fd *fd) {
    assert(fd != NULL);

    __stdio_lock(__clib4);

    if (fd->fd_Original != NULL) /* this is an alias */
    {
        struct fd *list_fd;

        assert(fd->fd_Original != fd);
        assert(fd->fd_Original->fd_Original == NULL);

        /* Remove this alias from the list. */
        for (list_fd = fd->fd_Original;
             list_fd != NULL;
             list_fd = list_fd->fd_NextAlias) {
            if (list_fd->fd_NextAlias == fd) {
                list_fd->fd_NextAlias = fd->fd_NextAlias;
                break;
            }
        }
    } else if (fd->fd_NextAlias != NULL) /* this one has aliases attached; it is the 'original' resource */
    {
        struct fd *first_alias;
        struct fd *next_alias;
        struct fd *list_fd;

        /* The first link now becomes the original resource */
        first_alias = fd->fd_NextAlias;
        next_alias = first_alias->fd_NextAlias;

        /* Structure copy... */
        (*first_alias) = (*fd);

        /* Fix up the linkage. */
        first_alias->fd_NextAlias = next_alias;
        first_alias->fd_Original = NULL;

        /* The resources are migrated to the first link. */
        for (list_fd = next_alias;
             list_fd != NULL;
             list_fd = list_fd->fd_NextAlias) {
            list_fd->fd_Original = first_alias;
        }
    }

    __stdio_unlock(__clib4);
}
