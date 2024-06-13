#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include "clib4.h"
#include "children.h"

BOOL
insertSpawnedChildren(uint32 pid, uint32 gid) {
    DECLARE_UTILITYBASE();

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (res) {
        struct Clib4Children *children = AllocVecTags(sizeof(struct Clib4Children), AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_DONE);
        if (children) {
            children->pid = pid;
            children->returnCode = 0;
            children->groupId = gid;
            struct SplayNode *node = InsertSplayNode(res->spawnedProcesses, pid, sizeof(*children));
            if (node) {
                node->sn_UserData = children;
                return TRUE;
            }
        }
    }
    return FALSE;
}