/*
 * $Id: stdio_initializefd.c,v 1.7 2006-10-10 13:39:26 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
__initialize_fd(
        struct fd *fd,
        file_action_fd_t action_function,
        BPTR default_file,
        ULONG flags,
        struct SignalSemaphore *lock) {
    ENTER();
    assert(fd != NULL && action_function != NULL);
    memset(fd, 0, sizeof(*fd));

    fd->fd_Version = 1;
    fd->fd_DefaultFile = default_file;
    fd->fd_Flags = flags;
    fd->fd_Action = action_function;
    fd->fd_Lock = lock;
    fd->fd_Aux = NULL;

    LEAVE();
}
