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
        uint32 me = GetPID(0, GPID_PROCESS);
        size_t iter = 0;
        void *item;

        struct Clib4Children children;
        children.pid = pid;
        children.returnCode = 0;
        children.groupId = gid;

        while (hashmap_iter(res->children, &iter, &item)) {
            const struct Clib4Node *node = item;
            if (node->pid == me) {
                hashmap_set(node->spawnedProcesses, &children);
                break;
            }
        }
        return TRUE;
    }
    return FALSE;
}

struct Clib4Children *
findSpawnedChildrenByPid(uint32 pid) {
    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (res) {
        uint32 me = GetPID(0, GPID_PROCESS);
        size_t iter = 0;
        void *item;

        while (hashmap_iter(res->children, &iter, &item)) {
            const struct Clib4Node *node = item;
            if (node->pid == me) {
                return (struct Clib4Children *) hashmap_scan_item(node->spawnedProcesses, pidChildrenScan, &pid);
            }
        }

    }
    return NULL;
}

struct Clib4Children *
findSpawnedChildrenByGid(uint32 pid, uint32 gid) {
    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (res) {
        uint32 me = GetPID(0, GPID_PROCESS);
        size_t iter = 0;
        void *item;

        while (hashmap_iter(res->children, &iter, &item)) {
            const struct Clib4Node *node = item;
            struct Clib4Children *children;
            if (node->pid == me) {
                children = hashmap_scan_item(node->spawnedProcesses, pidChildrenScan, &pid);
                if (children->groupId == gid)
                    return children;
            }
        }
    }
    return NULL;
}

void
spawnedProcessExit(int32 rc, int32 data UNUSED) {
    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (res) {
        int32 me = GetPID(0, GPID_PROCESS);
        size_t iter = 0;
        void *item;

        while (hashmap_iter(res->children, &iter, &item)) {
            const struct Clib4Node *node = item;
            if (node->pid == me) {
                struct Clib4Children key;
                key.pid = me;
                struct Clib4Children *item = (struct Clib4Children *) hashmap_get(node->spawnedProcesses, &key);
                if (item != NULL) {
                    item->returnCode = rc;
                }
            }
        }
    }
}

void
spawnedProcessFinal(int32 rc, int32 data UNUSED) {
    int32 pid = GetPID(0, GPID_PROCESS);
    D(("spawnedProcessFinal PID: %ld\n", pid));
}

