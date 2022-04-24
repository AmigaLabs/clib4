/*
 * $Id: stdio_change_fd_user_data.c,v 1.1 2006-11-15 09:17:04 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
__change_fd_user_data(
        int file_descriptor,
        void *new_user_data,
        void **old_user_data_ptr) {
    int result = -1;
    struct fd *fd;

    if (old_user_data_ptr != NULL)
        (*old_user_data_ptr) = NULL;

    __stdio_lock();

    fd = __get_file_descriptor(file_descriptor);
    if (fd != NULL) {
        __fd_lock(fd);

        if (old_user_data_ptr != NULL)
            (*old_user_data_ptr) = fd->fd_UserData;

        fd->fd_UserData = new_user_data;

        __fd_unlock(fd);

        result = 0;
    }

    __stdio_unlock();

    return (result);
}
