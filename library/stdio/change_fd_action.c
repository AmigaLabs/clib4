/*
 * $Id: stdio_change_fd_action.c,v 1.1 2006-11-15 09:17:04 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/****************************************************************************/

int
__change_fd_action(
        int file_descriptor,
        _file_action_fd_t new_action,
        _file_action_fd_t *old_action_ptr) {
    int result = -1;
    struct fd *fd;

    if (old_action_ptr != NULL)
        (*old_action_ptr) = NULL;

    __stdio_lock();

    fd = __get_file_descriptor(file_descriptor);
    if (fd != NULL) {
        __fd_lock(fd);

        if (old_action_ptr != NULL)
            (*old_action_ptr) = (_file_action_fd_t) fd->fd_Action;

        fd->fd_Action = (file_action_fd_t) new_action;

        __fd_unlock(fd);

        result = 0;
    }

    __stdio_unlock();

    return (result);
}
