/*
 * $Id: socket_closelog.c,v 1.0 2022-01-22 18:27:15 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

void
closelog(void) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    if (__clib4->syslog_fd != NULL) {
        fclose(__clib4->syslog_fd);
        __clib4->syslog_fd = NULL;
    }

    __check_abort();

    LEAVE();
}