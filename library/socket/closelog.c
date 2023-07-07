/*
 * $Id: socket_closelog.c,v 1.0 2022-01-22 18:27:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

void
closelog(void) {
    ENTER();
    struct _clib2 *__clib2 = __CLIB2;

    if (__clib2->syslog_fd != NULL) {
        fclose(__clib2->syslog_fd);
        __clib2->syslog_fd = NULL;
    }

    __check_abort();

    LEAVE();
}