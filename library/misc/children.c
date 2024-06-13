#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include "clib4.h"
#include "children.h"

static void *
gidChildrenScan(const void *children, void *gid) {
    const struct Clib4Children *myChildren = children;
    const int groupId = *((int *)gid);
    if (myChildren->groupId == groupId)
        return (struct Clib4Children *) children;
    return NULL;
}

static void *
pidChildrenScan(const void *children, void *pid) {
    const struct Clib4Children *myChildren = children;
    const int processId = *((int *)pid);
    if (myChildren->pid == processId)
        return (struct Clib4Children *) children;
    return NULL;
}

BOOL
insertSpawnedChildren(uint32 pid, uint32 gid) {
    DECLARE_UTILITYBASE();

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (res) {
        struct Clib4Children children;
        children.pid = pid;
        children.returnCode = 0;
        children.groupId = gid;
        hashmap_set(res->spawnedProcesses, &children);
        return TRUE;
    }
    return FALSE;
}

struct Clib4Children *
findSpawnedChildrenByPid(uint32 pid) {
    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (res) {
        return (struct Clib4Children *) hashmap_scan_item(res->spawnedProcesses, pidChildrenScan, &pid);
    }
    return NULL;
}

struct Clib4Children *
findSpawnedChildrenByGid(uint32 pid, uint32 gid) {
    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (res) {
        struct Clib4Children *children = hashmap_scan_item(res->spawnedProcesses, pidChildrenScan, &pid);
        if (children->groupId == gid)
            return children;
    }
    return NULL;
}