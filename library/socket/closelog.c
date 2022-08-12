/*
 * $Id: socket_closelog.c,v 1.0 2022-01-22 18:27:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */


void
closelog(void) {
    ENTER();

    struct SignalSemaphore *lock = NULL;

    if (syslog_fd != NULL) {
        lock = __create_semaphore();
        if (lock == NULL) {
            __set_errno(ENOMEM);
            goto out;
        }

        fclose(syslog_fd);
        syslog_fd = NULL;

        __delete_semaphore(lock);
    }

out:
    __check_abort();

    LEAVE();
}