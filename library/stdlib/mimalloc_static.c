/*
 * Embed mimalloc as a static translation unit for clib4.
 */

#define MI_STATIC_LIB 1

/* AmigaOS4 compatibility: mimalloc unix backend expects madvise APIs that are
 * not always available in the SDK headers. */
#ifndef MADV_DONTNEED
#define MADV_DONTNEED 0
#endif
#ifndef MADV_FREE
#define MADV_FREE MADV_DONTNEED
#endif
#ifndef MADV_HUGEPAGE
#define MADV_HUGEPAGE 0
#endif
#ifndef madvise
#define madvise(addr, size, advice) (0)
#endif

#include "../../third_party/mimalloc/src/static.c"
