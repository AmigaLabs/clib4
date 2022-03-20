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

int32 MemalignAVLNodeComp(struct AVLNode *avlnode1, struct AVLNode *avlnode2)
{
    struct MemalignEntry *e1, *e2;

    e1 = (struct MemalignEntry *)avlnode1;
    e2 = (struct MemalignEntry *)avlnode2;

    return (int32)((uint32)e1->me_Aligned - (uint32)e2->me_Aligned);
}

int32 MemalignAVLKeyComp(struct AVLNode *avlnode1, AVLKey key2)
{
    struct MemalignEntry *e1 = (struct MemalignEntry *)avlnode1;

    return (int32)((uint32)e1->me_Aligned - (uint32)key2);
}

static inline BOOL isPowerOfTwo(size_t alignment)
{
    return (alignment != 0) && ((alignment & (alignment - 1)) == 0);
}

void *
memalign(size_t alignment, size_t size)
{
    void *result;
    unsigned long int adj;

    if (!isPowerOfTwo(alignment))
    {
        __set_errno(EINVAL);
        return NULL;
    }

    if (__global_clib2->__memalign_pool == NULL) {
        __set_errno(ENOSYS);
        return NULL;
    }
    size = ((size + alignment - 1) / alignment) * alignment;

    result = malloc(size);
    if (result == NULL)
        return NULL;

    adj = (unsigned long int)((unsigned long int)((char *)result - (char *)NULL)) % alignment;
    /* if block is not aligned, align it */
    if (adj != 0)
    {
        struct MemalignEntry *l = ItemPoolAlloc(__global_clib2->__memalign_pool);
        if (l == NULL)
        {
            free(result);
            return NULL;
        }
        if (NULL != AVL_AddNode(&__global_clib2->__memalign_tree, &l->me_AvlNode, MemalignAVLNodeComp))
        {
            FreeVec(result);
            ItemPoolFree(__global_clib2->__memalign_pool, l);
            result = NULL;
            goto out;
        }

        /* Set MemalignEntry node stuff */
        l->me_Exact = result;
        l->me_Aligned = (char *)result + alignment - adj;
        result = l->me_Aligned;
    }

out:
    return result;
}
