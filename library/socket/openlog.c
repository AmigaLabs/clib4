/*
 * $Id: socket_openlog.c,v 1.0 2022-01-22 18:27:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include <syslog.h>

FILE *syslog_fd = NULL;
int syslog_openlog_flags = 0;
char syslog_ident[35] = {0};

void
openlog(const char *ident, int opt, int facility) {
    (void) facility;

    ENTER();

    syslog_openlog_flags = opt;

    if (syslog_fd == NULL) {
        if (strlen(ident) + 3 > 35) {
            SHOWMSG("ident is too long");

            __set_errno(ENAMETOOLONG);
            goto out;
        }
        if (ident)
            snprintf(syslog_ident, sizeof(syslog_ident), "%s%s", _PATH_LOG, ident);
        else
            snprintf(syslog_ident, sizeof(syslog_ident), "%sDUMMY", _PATH_LOG, ident);

        Printf("Opening %s\n", syslog_ident);
        if ((syslog_fd = fopen(syslog_ident, "a+")) == NULL) {
            SHOWMSG("Error opening syslog file");
            if (syslog_openlog_flags & LOG_CONS) {
                fprintf(stderr, "Error opening syslog file %s\n", syslog_ident);
            }
        }
    }

out:

    LEAVE();
}