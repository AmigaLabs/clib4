/*

 */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/*
 * You can switch the built-in memory allocator, which is a thin wrapper
 * around the AmigaOS built-in memory management system, to use a slab
 * allocator. For this to work, you need to declare a global variable
 * and set it to the size of the slabs to be used.
 * It is enabled by default. If you want to disable replace this line with
 *
 * extern unsigned long __slab_max_size;
 */

unsigned long __slab_max_size = 8192;
