/*
 * $Id: socket_openlog.c,v 1.0 2022-01-22 18:27:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

void
openlog(const char *ident, int opt, int facility) {
    ENTER();
    struct _clib2 *__clib2 = __CLIB2;

    __clib2->syslog_openlog_flags = opt;
    __clib2->syslog_facility = facility;

    if (__clib2->syslog_fd == NULL) {
        if (strlen(ident) + 3 > 35) {
            SHOWMSG("ident is too long");

            __set_errno(ENAMETOOLONG);
            goto out;
        }

        if (ident)
            snprintf(__clib2->syslog_ident, sizeof(__clib2->syslog_ident), "%s%s", _PATH_LOG, ident);
        else
            snprintf(__clib2->syslog_ident, sizeof(__clib2->syslog_ident), "%sDUMMY", _PATH_LOG, ident);

        if ((__clib2->syslog_fd = fopen(__clib2->syslog_ident, "a+")) == NULL) {
            SHOWMSG("Error opening syslog file");
            if (__clib2->syslog_openlog_flags & LOG_CONS) {
                fprintf(stderr, "Error opening syslog file %s\n", __clib2->syslog_ident);
            }
        }
    }

out:

    __check_abort();

    LEAVE();
}