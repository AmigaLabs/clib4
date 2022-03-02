/*
 * $Id: malloc_memalign.c,v 1.0 2021-01-10 10:52:18 apalmate Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************
 *
 * Documentation and source code for this library, and the most recent library
 * build are available from <https://github.com/afxgroup/clib2>.
 *
 *****************************************************************************
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
