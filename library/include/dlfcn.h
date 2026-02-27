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

typedef struct dl_info {
    const char      *dli_fname;     /* Pathname of shared object */
    void            *dli_fbase;     /* Base address of shared object */
    const char      *dli_sname;     /* Name of nearest symbol */
    void            *dli_saddr;     /* Address of nearest symbol */
} Dl_info;

extern int dladdr(const void *addr, Dl_info *info);
extern int dlclose(void *__handle);
extern const char *dlerror(void);
extern void *dlopen(const char *__path_name, int __mode);
extern void *dlsym(void *__handle, const char *__symbol_name);

__END_DECLS

#endif /* _DLFCN_H */
