/*
 * $Id: malloc_memalign.c,v 1.0 2021-01-10 10:52:18 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <stdint.h>
#include <malloc.h>

int32
MemalignAVLNodeComp(struct AVLNode *avlnode1, struct AVLNode *avlnode2) {
    struct MemalignEntry *e1, *e2;

    e1 = (struct MemalignEntry *) avlnode1;
    e2 = (struct MemalignEntry *) avlnode2;

    return (int32)((uint32) e1->me_Exact - (uint32) e2->me_Exact);
}

int32
MemalignAVLKeyComp(struct AVLNode *avlnode1, AVLKey key2) {
    struct MemalignEntry *e1 = (struct MemalignEntry *) avlnode1;

    return (int32)((uint32) e1->me_Exact - (uint32) key2);
}

static inline BOOL
isPowerOfTwo(size_t alignment) {
    return (alignment != 0) && ((alignment & (alignment - 1)) == 0);
}

void *
memalign(size_t alignment, size_t size) {
    void *result = NULL;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    SHOWVALUE(alignment);
    SHOWVALUE(size);

    if (!isPowerOfTwo(alignment)) {
        __set_errno(EINVAL);
        goto out;
    }

    if (__clib2->__memalign_pool == NULL) {
        __set_errno(ENOSYS);
        goto out;
    }
    //size = ((size + alignment - 1) / alignment) * alignment;

    result = AllocVecTags(size, AVT_Type, MEMF_SHARED, AVT_Alignment, alignment, TAG_END);
    if (result == NULL) {
        __set_errno(ENOMEM);
        goto out;
    }

    struct MemalignEntry *l = ItemPoolAlloc(__clib2->__memalign_pool);
    if (l == NULL) {
        FreeVec(result);
        __set_errno(ENOMEM);
        result = NULL;
        goto out;
    }
    /* Set MemalignEntry node stuff */
    l->me_Exact = result;

    if (NULL != AVL_AddNode(&__clib2->__memalign_tree, &l->me_AvlNode, MemalignAVLNodeComp)) {
        FreeVec(result);
        ItemPoolFree(__clib2->__memalign_pool, l);
        __set_errno(ENOMEM);
        result = NULL;
        goto out;
    }

out:
    RETURN(result);
    return result;
}
