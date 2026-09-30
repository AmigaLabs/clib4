/*
 * Compatibility shim to build the glibc-derived wordexp.c on clib4.
 * Provides the glibc-internal macros/functions that clib4 lacks.
 * Not installed.
 */

#ifndef _WORDEXP_GLIBC_COMPAT_H
#define _WORDEXP_GLIBC_COMPAT_H

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* glibc internal linkage / attributes */
#ifndef internal_function
#define internal_function
#endif
#ifndef attribute_hidden
#define attribute_hidden
#endif
#ifndef libc_hidden_def
#define libc_hidden_def(x)
#endif
#ifndef weak_alias
#define weak_alias(a, b)
#endif

/* glibc double-underscore aliases -> plain POSIX */
#ifndef __getpid
#define __getpid getpid
#define __geteuid geteuid
#define __getuid getuid
#define __getegid getegid
#define __getgid getgid
#define __fork fork
#define __close close
#define __read read
#define __dup2 dup2
#define __open open
#define __fcntl fcntl
#define __pipe pipe
#define __kill kill
#define __waitpid waitpid
#define __execve execve
#define __unsetenv unsetenv
#define __environ environ
#define __fxstat64(v, f, s) fstat(f, s)
#define __getpwuid_r getpwuid_r
#define __getpwnam_r getpwnam_r
#define __strcasecmp strcasecmp
#define __strndup strndup
#define __strdup strdup
#define __free free
#endif

/* _STAT_VER used by __fxstat64 on glibc */
#ifndef _STAT_VER
#define _STAT_VER 0
#endif

/* Retry an interrupted syscall */
#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression) \
    (__extension__ ({ long int __result; \
        do __result = (long int)(expression); \
        while (__result == -1L && errno == EINTR); \
        __result; }))
#endif

/* gettext no-op */
#ifndef _
#define _(msgid) (msgid)
#endif

/* _itoa: write integer as decimal, return pointer past the end */
#ifndef _itoa_word
static inline char *
_clib4_itoa_word(unsigned long value, char *buflim, int base, int uppercase) {
    const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    (void)digits;
    /* buflim points past the end; write backwards. Only base 10 is used.  */
    (void)base;
    do {
        *--buflim = '0' + (value % 10);
        value /= 10;
    } while (value != 0);
    return buflim;
}
#define _itoa_word(value, buflim, base, uppercase) \
    _clib4_itoa_word(value, buflim, base, uppercase)
#endif

/* glibc _itoa(value, base, ndigit, uppercase) -> decimal string into a
 * static-size buffer. clib4 wordexp only uses base 10. */
#include <stdio.h>
#define _itoa(value, base, ndigit, uppercase) _clib4_itoa((value))
static inline char *
_clib4_itoa(long value) {
    static char buf[32];
    snprintf(buf, sizeof(buf), "%ld", value);
    return buf;
}

/* bool */
#ifndef __cplusplus
#include <stdbool.h>
#endif

/* glibc mempcpy / stpcpy variants */
#ifndef __mempcpy
#define __mempcpy mempcpy
#endif
#ifndef __stpcpy
#define __stpcpy stpcpy
#endif

/* alloca helpers */
#include <alloca.h>
#ifndef __alloca
#define __alloca alloca
#endif
#ifndef extend_alloca
#define extend_alloca(buf, len, newlen) \
    (__extension__ ({ char *__new = (char *) alloca((newlen)); \
        memcpy(__new, (buf), (len)); __new; }))
#endif
#ifndef strndupa
#define strndupa(s, n) \
    (__extension__ ({ const char *__old = (s); size_t __len = strnlen(__old, (n)); \
        char *__new = (char *) alloca(__len + 1); \
        __new[__len] = '\0'; \
        (char *) memcpy(__new, __old, __len); }))
#endif

/* locked stdio -> plain stdio */
#ifndef __fxprintf
#define __fxprintf(stream, fmt, ...) fprintf((stream), (fmt), ##__VA_ARGS__)
#endif

#ifndef __setenv
#define __setenv setenv
#endif

/* paths */
#ifndef _PATH_BSHELL
#define _PATH_BSHELL "C:sh"
#endif
#ifndef _PATH_DEVNULL
#define _PATH_DEVNULL "NIL:"
#endif

/* device-nrs.h not present: the /dev/null major/minor check is disabled */
#undef DEV_NULL_MAJOR
#undef DEV_NULL_MINOR

/* libc lock: wordexp here is not thread-reentrant-guarded */
#define __libc_lock_define_initialized(CLASS, NAME)
#define __libc_lock_lock(NAME)
#define __libc_lock_unlock(NAME)

extern char **environ;

#endif /* _WORDEXP_GLIBC_COMPAT_H */
