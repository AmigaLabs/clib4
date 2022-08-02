/*
 * $Id: socket_closelog.c,v 1.0 2022-01-22 18:27:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include <syslog.h>

void
closelog(void) {

    ENTER();

    if (syslog_fd != NULL) {
        fclose(syslog_fd);
        syslog_fd = NULL;
        Printf("Close log\n");
    }

    LEAVE();
}