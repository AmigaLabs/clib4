/*
* $Id: execinfo.h,v 1.0 2025-03-02 12:06:14 clib4devs Exp $
*/

#ifndef _EXECINFO_H
#define _EXECINFO_H

#include <features.h>

__BEGIN_DECLS

/* Store up to SIZE return address of the current program state in
   ARRAY and return the exact number of values stored.  */
extern int backtrace(void **__array, int __size);
extern char **backtrace_symbols(void *const *buffer, int size);
extern void backtrace_symbols_fd(void *const *buffer, int size, int fd);

__END_DECLS

#endif
