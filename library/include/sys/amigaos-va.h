/*
 * $Id: amigaos-va.h,v 1.7 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _SYS_AMIGAOS_VA_H
#define _SYS_AMIGAOS_VA_H

#ifndef _STDARG_H
#include <stdarg.h>
#endif /* _STDARG_H */

#include <features.h>

__BEGIN_DECLS

/* Define the AmigaOS specific varargs macros,
   if <stdarg.h> didn't already define them.   */

#ifndef va_startlinear
 #define va_startlinear(AP,ARG) \
	va_start(AP,ARG)
#endif

#ifndef va_getlinearva
 #if defined(__GNUC__) && defined(__PPC__)
  #define va_getlinearva(AP,TYPE) \
	((TYPE)__builtin_getlinearva(AP))
 #else
  #define va_getlinearva(AP,TYPE) \
	((TYPE)(void *)(AP))
 #endif
#endif

__END_DECLS

#endif /* _SYS_AMIGAOS_VA_H */
