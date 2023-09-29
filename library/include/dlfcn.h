/*
 * $Id: dlfcn.h,v 1.2 2010-08-21 11:37:03 clib4devs Exp $
*/

#ifndef _DLFCN_H
#define _DLFCN_H

#include <features.h>

__BEGIN_DECLS

/* We only support a subset of the flags available on Unix systems. */
#define RTLD_LAZY       1
#define RTLD_NOW        2
#define RTLD_LOCAL      4
#define RTLD_GLOBAL     8
#define RTLD_DEFAULT    ((void *)0)

extern int dlclose(void *__handle);
extern const char *dlerror(void);
extern void *dlopen(const char *__path_name, int __mode);
extern void *dlsym(void *__handle, const char *__symbol_name);

__END_DECLS

#endif /* _DLFCN_H */
