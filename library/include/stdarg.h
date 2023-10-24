/*
 * $Id: stdarg.h,v 1.7 2006-01-08 12:06:14 clib4devs Exp $
*/

#ifndef _STDARG_H
#define _STDARG_H

#include <features.h>

__BEGIN_DECLS

typedef __builtin_va_list va_list;

#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#define va_copy(d,s)    __builtin_va_copy(d,s)

#include <sys/amigaos-va.h>

__END_DECLS

#endif /* _STDARG_H */
