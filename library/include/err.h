#ifndef _ERR_H
#define _ERR_H

#include <features.h>
#include <stdarg.h>

__BEGIN_DECLS

extern void warn(const char *, ...);
extern void vwarn(const char *, va_list);
extern void warnx(const char *, ...);
extern void vwarnx(const char *, va_list);

extern void err(int, const char *, ...);
extern void verr(int, const char *, va_list);
extern void errx(int, const char *, ...);
extern void verrx(int, const char *, va_list);

__END_DECLS

#endif