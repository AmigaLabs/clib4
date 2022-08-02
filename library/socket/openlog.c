/*
 * $Id: socket_openlog.c,v 1.0 2022-01-22 18:27:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

FILE *syslog_fd = NULL;
int syslog_openlog_flags = 0;
char syslog_ident[35] = {0};
int syslog_facility = LOG_USER;

void
openlog(const char *ident, int opt, int facility) {
    ENTER();

    struct SignalSemaphore *lock = NULL;

    syslog_openlog_flags = opt;
    syslog_facility = facility;

    if (syslog_fd == NULL) {
        if (strlen(ident) + 3 > 35) {
            SHOWMSG("ident is too long");

            __set_errno(ENAMETOOLONG);
            goto out;
        }

        lock = __create_semaphore();

        if (ident)
            snprintf(syslog_ident, sizeof(syslog_ident), "%s%s", _PATH_LOG, ident);
        else
            snprintf(syslog_ident, sizeof(syslog_ident), "%sDUMMY", _PATH_LOG, ident);

        if ((syslog_fd = fopen(syslog_ident, "a+")) == NULL) {
            SHOWMSG("Error opening syslog file");
            if (syslog_openlog_flags & LOG_CONS) {
                fprintf(stderr, "Error opening syslog file %s\n", syslog_ident);
            }
        }
    }

out:

    if (lock != NULL)
        __delete_semaphore(lock);

    if (__check_abort_enabled)
        __check_abort();

    LEAVE();
}