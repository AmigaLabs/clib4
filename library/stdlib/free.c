/*
 * $Id: stdlib_free.c,v 1.15 2022-04-03 12:04:25 clib4devs Exp $
*/

/*#define DEBUG*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#include <malloc.h>

void
__free_r(struct _clib4 *__clib4, void *ptr) {
    __memory_lock(__clib4);

    BOOL found = FALSE;
    struct MemalignEntry *e = NULL;
    /* Check if we have something created by memalign */
    if (__clib4 != NULL) {
        e = (struct MemalignEntry *) AVL_FindNode(__clib4->__memalign_tree, ptr, MemalignAVLKeyComp);
        if (e) {
            found = TRUE;
        }
    }

    if (found) {
        /* Free memory */
        FreeVec(e->me_Exact);
        e->me_Exact = NULL;
        /* Remove the node */
        AVL_RemNodeByAddress(&__clib4->__memalign_tree, &e->me_AvlNode);
        ItemPoolFree(__clib4->__memalign_pool, e);
        e = NULL;
    } else {
        wof_free(__clib4->__wof_allocator, ptr);
    }

    __memory_unlock(__clib4);
}

void
free(void *ptr) {
    struct _clib4 *__clib4 = __CLIB4;

    return __free_r(__clib4, ptr);
}
