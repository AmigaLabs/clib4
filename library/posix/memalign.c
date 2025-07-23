/*
 * $Id: malloc_memalign.c,v 1.1 2025-06-07 10:52:18 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <stdint.h>
#include <malloc.h>

// static inline BOOL
// isPowerOfTwo(size_t alignment) {
//     return (alignment != 0) && ((alignment & (alignment - 1)) == 0);
// }

void *
memalign(size_t alignment, size_t size) {

    // Alignment must be powers of two
    if (alignment == 0 || (alignment & -alignment) != alignment) {
        __set_errno(EINVAL);
        return NULL;
    }

    return __malloc_aligned_r(__CLIB4, size, alignment);

//     uintptr_t aligned_addr = 0;
//     struct _clib4 *__clib4 = __CLIB4;

//     ENTER();

//     SHOWVALUE(alignment);
//     SHOWVALUE(size);

//     // Validate alignment
//     if (!isPowerOfTwo(alignment)) {
//         __set_errno_r(__clib4, EINVAL);
//         goto out;
//     }

//     // Architecture-specific minimum alignment
//     const size_t min_alignment = sizeof(void*) * 2;  // 8 on 32-bit, 16 on 64-bit
//     if (alignment < min_alignment)
//         alignment = min_alignment;

//     const size_t header_size = sizeof(AlignedHeader);
//     const size_t padding = alignment - 1;

//     // Prevent overflow
//     if (size > SIZE_MAX - (header_size + padding)) {
//         __set_errno_r(__clib4, EOVERFLOW);
//         goto out;
//     }

//     void* original_ptr = __malloc_r(__clib4, size + header_size + padding);
//     if (!original_ptr) {
//         __set_errno_r(__clib4, ENOMEM);
//         goto out;
//     }

//     // Calculate aligned address
//     uintptr_t base = (uintptr_t)original_ptr;
//     aligned_addr = (base + header_size + alignment - 1) & ~(alignment - 1);

//     // Store metadata
//     AlignedHeader* header = (AlignedHeader*)(aligned_addr - header_size);
//     header->magic = ALIGNED_BLOCK_MAGIC;
//     header->original_ptr = original_ptr;

// out:
//     RETURN(aligned_addr);
//     return (void *) aligned_addr;
}
