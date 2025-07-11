/*
 * $Id: socket_vsyslog.c,v 1.0 2022-01-22 18:27:15 clib4devs Exp $
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
    struct _clib4 *__clib4 = __CLIB4;

    if (!(__clib4->syslog_mask & LOG_MASK(priority & 7)) || (priority & ~0x3ff))
        return;

    int l, l2, hlen, pid;
    char timebuf[16] = {0};
    time_t now;
    struct tm tm;
    char buf[1024] = {0};

    if (!(priority & LOG_FACMASK))
        priority |= __clib4->syslog_facility;

    now = time(NULL);
    gmtime_r(&now, &tm);
    strftime(timebuf, sizeof timebuf, "%b %e %T", &tm);
    pid = (__clib4->syslog_openlog_flags & LOG_PID) ? getpid() : 0;
    l = snprintf(buf, sizeof buf, "<%s>\t%s %n%s%s%.0d%s: ", prioritynames[priority].c_name, timebuf, &hlen, __clib4->syslog_ident, "[" + !pid, pid, "]" + !pid);

    l2 = vsnprintf(buf + l, sizeof buf - l, message, args);
    if (l2 >= 0) {
        if ((size_t) l2 >= sizeof buf - l)
            l = sizeof buf - 1;
        else
            l += l2;
        if (buf[l - 1] != '\n') buf[l++] = '\n';

        if (__clib4->syslog_fd != NULL) {
            fprintf(__clib4->syslog_fd, "%s", &buf);
        }
        else {
            __vsyslog(priority, (char *) message, args);
        }
    }
    LEAVE();
}