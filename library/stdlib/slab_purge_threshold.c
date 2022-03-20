/*

 */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/*
 * The slab allocator will periodically free all currently unused memory.
 * You can control how much memory should be released, instead of
 * releasing everything.
 *
 * This would make the slab allocator release only up to 512 KBytes of
 * unused memory at a time:
 *
 * unsigned long __slab_purge_threshold = 512 * 1024;
 *
 * If you want to disable the purge set it as:
 * extern unsigned long __slab_purge_threshold;
 */

unsigned long __slab_purge_threshold = 0;
