/*
 * $Id: stdio_err.c,v 1.0 2022-07-13 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#include <err.h>

void vwarn(const char *fmt, va_list ap) {
    fprintf(stderr, "%s: ", __CLIB4->__progname);
    if (fmt) {
        vfprintf(stderr, fmt, ap);
        fputs(": ", stderr);
    }
    perror(0);
}

void vwarnx(const char *fmt, va_list ap) {
    fprintf(stderr, "%s: ", __CLIB4->__progname);
    if (fmt) vfprintf(stderr, fmt, ap);
    putc('\n', stderr);
}

void verr(int status, const char *fmt, va_list ap) {
    vwarn(fmt, ap);
    exit(status);
}

void verrx(int status, const char *fmt, va_list ap) {
    vwarnx(fmt, ap);
    exit(status);
}

void warn(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwarn(fmt, ap);
    va_end(ap);
}

void warnx(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwarnx(fmt, ap);
    va_end(ap);
}

void err(int status, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    verr(status, fmt, ap);
    va_end(ap);
}

void errx(int status, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    verrx(status, fmt, ap);
    va_end(ap);
}
