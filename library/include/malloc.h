/*
 * $Id: malloc.h,v 1.0 2021-01-10 10:50:49 clib4devs Exp $
*/

#ifndef _MALLOC_H
#define _MALLOC_H

#include <features.h>

#include <stddef.h>
#include <stdio.h>

#define SIZE_ALIGN (4 * sizeof(size_t))
#define SIZE_MASK (-SIZE_ALIGN)

__BEGIN_DECLS

/* Allocate SIZE bytes allocated to ALIGNMENT bytes.  */
extern void *memalign(size_t alignment, size_t size);

__END_DECLS

#endif //_MALLOC_H
