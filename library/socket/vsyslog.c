/*
 * $Id: socket_vsyslog.c,v 1.0 2022-01-22 18:27:15 clib2devs Exp $
*/
#define SYSLOG_NAMES

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */


void
vsyslog(int priority, const char *message, va_list args) {
    ENTER();

    SHOWVALUE(priority);
    SHOWSTRING(message);

    if (!(syslog_mask & LOG_MASK(priority & 7)) || (priority & ~0x3ff))
        return;

    int l, l2, hlen, pid;
    char timebuf[16] = {0};
    time_t now;
    struct tm tm;
    char buf[1024] = {0};

    if (!(priority & LOG_FACMASK))
        priority |= syslog_facility;

    now = time(NULL);
    gmtime_r(&now, &tm);
    strftime(timebuf, sizeof timebuf, "%b %e %T", &tm);
    pid = (syslog_openlog_flags & LOG_PID) ? getpid() : 0;
    l = snprintf(buf, sizeof buf, "<%s>\t%s %n%s%s%.0d%s: ", prioritynames[priority].c_name, timebuf, &hlen, syslog_ident, "[" + !pid, pid, "]" + !pid);

    l2 = vsnprintf(buf + l, sizeof buf - l, message, args);
    if (l2 >= 0) {
        if (l2 >= sizeof buf - l)
            l = sizeof buf - 1;
        else
            l += l2;
        if (buf[l - 1] != '\n') buf[l++] = '\n';

        if (syslog_fd != NULL) {
            fprintf(syslog_fd, "%s", &buf);
        }
        else {
            __vsyslog(priority, (char *) message, args);
        }
    }
    LEAVE();
}