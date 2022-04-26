/*
 * $Id: stdlib_free.c,v 1.15 2022-04-03 12:04:25 clib2devs Exp $
*/

/*#define DEBUG*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#include <malloc.h>

#undef free

void
free(void *ptr) {

    BOOL found = FALSE;
    struct MemalignEntry *e = NULL;
    /* Check if we have something created by memalign */
    if (__global_clib2 != NULL) {
        e = (struct MemalignEntry *) AVL_FindNode(__global_clib2->__memalign_tree, ptr, MemalignAVLKeyComp);
        if (e) {
            found = TRUE;
        }
    }

    __memory_lock();

    if (found) {
        /* Free memory */
        FreeVec(e->me_Exact);
        e->me_Exact = NULL;
        /* Remove the node */
        AVL_RemNodeByAddress(&__global_clib2->__memalign_tree, &e->me_AvlNode);
        ItemPoolFree(__global_clib2->__memalign_pool, e);
        e = NULL;
    } else {
        wof_free(__wof_allocator, ptr);
    }

    __memory_unlock();
}
