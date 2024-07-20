/*
 * $Id: stdlib_memory.h,v 1.4 2006-01-08 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_MEMORY_H
#define _STDLIB_MEMORY_H

#ifndef	EXEC_MEMORY_H
#include <exec/memory.h>
#endif /* EXEC_MEMORY_H */

#ifndef EXEC_NODES_H
#include <exec/nodes.h>
#endif /* EXEC_NODES_H */

/****************************************************************************/

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

#include <stddef.h>

#include "wof_allocator.h"

/* We shuffle things around a bit for the debug code. This works by joining
   related code which shares the same name. The debug code symbols also have
   to be completely separate from the "regular" code. */

#define __static STATIC

struct AVLMemoryNode {
    /* Variables used to track nodes and free them */
    struct AVLNode  amn_AvlNode;
    void           *amn_Address;
    ULONG           amn_Size;
};

/* At moment to align memory we use AllocVectTags that already align the memory
 * The MemalignEntry structure is used when we call memalign and we have two different
 * ways to free memory. In our case if free is called via memalign
 * we have to use FreeVec otherwise we have to use wof_free
 */
struct MemalignEntry {
    struct AVLNode   me_AvlNode;
    void            *me_Exact;            /* The address returned by AllocVecTags  */
};

extern void __memory_lock(struct _clib4 *__clib4);
extern void __memory_unlock(struct _clib4 *__clib4);

extern void __free_r(struct _clib4 *__clib4, void *ptr);


extern int32 MemalignAVLNodeComp(struct AVLNode *avlnode1, struct AVLNode *avlnode2);
extern int32 MemalignAVLKeyComp(struct AVLNode *avlnode1, AVLKey key2);

#endif /* _STDLIB_MEMORY_H */
