#ifndef _ERR_H
#define _ERR_H

#include <features.h>
#include <stdarg.h>

__BEGIN_DECLS

void warn(const char *, ...);
void vwarn(const char *, va_list);
void warnx(const char *, ...);
void vwarnx(const char *, va_list);

extern void err(int, const char *, ...);
extern void verr(int, const char *, va_list);
extern void errx(int, const char *, ...);
extern void verrx(int, const char *, va_list);

__END_DECLS

#endif