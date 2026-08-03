/*
 * $Id: stdlib_malloc_dump.c,v 1.0 2026-07-13 12:00:00 clib4devs Exp $
 *
 * clib4 extension: dump the internal state of the per-process wmem
 * allocator on the serial port via DebugPrintF(). Walks every OS block,
 * chunk and live allocation known to the allocator and prints summary
 * totals. Useful to inspect memory usage and hunt leaks at any point of
 * a program's lifetime (capture the output with a serial terminal or
 * Sashimi/DebugPrintF redirectors).
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#include <malloc.h>

void
malloc_dump(void) {
    struct _clib4 *__clib4 = __CLIB4;

    __memory_lock(__clib4);

    wmem_dump_allocator(__get_wmem_allocator(__clib4));

    __memory_unlock(__clib4);
}
