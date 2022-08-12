#include <syslog.h>
#include <stdio.h>
#include <stdarg.h>

void
debug(char *message, ...) {
    va_list ap;

    openlog("TEST", LOG_PID | LOG_CONS, LOG_DAEMON);

    va_start(ap, message);
    vsyslog(LOG_DEBUG, message, ap);
    va_end(ap);

    closelog();
}

int main() {
    syslog(LOG_DEBUG, "Test message. %d + %d = %d", 1, 2, 3);
    debug("Test message. %d + %d = %d", 1, 2, 3);
    return 0;
}