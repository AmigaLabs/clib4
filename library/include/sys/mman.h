/*
 * $Id: mman.h,v 1.0 2021-01-18 20:17:27 clib2devs Exp $
*/

#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <features.h>

__BEGIN_DECLS

#include <sys/types.h> // off_t

/* Return value of `mmap' in case of an error.  */
#define MAP_FAILED      ((void *) -1)

#define PROT_READ       0x1             /* page can be read */
#define PROT_WRITE      0x2             /* page can be written */
#define PROT_EXEC       0x4             /* page can be executed */
#define PROT_SEM        0x8             /* page may be used for atomic ops */
#define PROT_NONE       0x0             /* page can not be accessed */

#define MAP_SHARED      0x01            /* Share changes */
#define MAP_PRIVATE     0x02            /* Changes are private */
#define MAP_SHARED_VALIDATE 0x03        /* share + validate extension flags */
#define MAP_TYPE        0x0f            /* Mask for type of mapping */
#define MAP_FIXED       0x10            /* Interpret addr exactly */
#define MAP_ANONYMOUS   0x20            /* don't use a file */

extern void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
extern int munmap(void *map, size_t length);
extern int msync(void *addr, size_t len, int flags);

__END_DECLS

#endif // _SYS_MMAN_H