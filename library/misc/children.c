/*
  $Id: uuid.c,v 1.01 2024-08-28 12:09:49 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

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

static void *
pipeChildrenScan(const void *children, void *pipe) {
    const struct Clib4Children *myChildren = children;
    FILE *stream = *((FILE **)pipe);
    if (myChildren->pipe == stream)
        return (struct Clib4Children *) children;
    return NULL;
}

BOOL
insertSpawnedChildren(uint32 pid, uint32 ppid, uint32 gid) {
    DECLARE_UTILITYBASE();

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (res) {
        uint32 parent = ppid; //GetPID(0, GPID_PARENT);
        size_t iter = 0;
        void *item;

        struct Clib4Children children;
        children.pid = pid;
        children.returnCode = 0x80000000; //set this flag for WIFEXITED
        children.groupId = gid;

        while (hashmap_iter(res->children, &iter, &item)) {
            const struct Clib4Node *node = item;
            if (node->pid == parent) {
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
        uint32 mainProcess = GetPID(0, GPID_PARENT) != 0 ? GetPID(0, GPID_PARENT) : GetPID(0, GPID_PROCESS);
        size_t iter = 0;
        void *item;

        while (hashmap_iter(res->children, &iter, &item)) {
            const struct Clib4Node *node = item;
            if (node->pid == mainProcess) {
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
            // struct Clib4Children *children;
            if (node->pid == me) {
                //children =
                return hashmap_scan_item(node->spawnedProcesses, gidChildrenScan, &gid);
            }
        }
    }
    return NULL;
}

pid_t
findSpawnedChildrenPidByPipe(FILE *pipe) {
    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (res) {
        uint32 me = GetPID(0, GPID_PROCESS);
        size_t iter = 0;
        void *item;

        while (hashmap_iter(res->children, &iter, &item)) {
            const struct Clib4Node *node = item;
            struct Clib4Children *children;
            if (node->pid == me) {
                children = hashmap_scan_item(node->spawnedProcesses, pipeChildrenScan, &pipe);
                if(children) return children->pid;
            }
        }
    }
    return -1;
}

void
addSpawnedChildrenPipeHandle(uint32 pid, FILE *pipe) {
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
                if (children) children->pipe = pipe;
                break;
            }
        }
    }
}

void
spawnedProcessEnter(int32 entry_data) {
    struct Library *UserGroupBase = 0;
    struct UserGroupIFace *IUserGroup = 0;
    gid_t groupId = (gid_t)entry_data;

#if 0
    UserGroupBase = OpenLibrary("usergroup.library", 0);

    if (UserGroupBase != NULL) {
        IUserGroup = (struct UserGroupIFace *)GetInterface(UserGroupBase, "main", 1, 0);
        if (IUserGroup == NULL) {
            UserGroupBase = NULL;
        }
        else {
            groupId = IUserGroup->getgid();
            DropInterface((struct Interface *) IUserGroup);
        }
        CloseLibrary(UserGroupBase);
    }
#endif

    uint32 pid = GetPID(0, GPID_PROCESS); //((struct Process *) FindTask(NULL))->pr_ProcessID;
    uint32 ppid = GetPID(0, GPID_PARENT);
    if (insertSpawnedChildren(pid, ppid, groupId)) {
        __CLIB4->__children++;
        D(("Children with pid %ld and gid %ld inserted into list\n", pid, groupId));
    }
    else {
        D(("Cannot insert children with pid %ld and gid %ld into list\n", pid, groupId));
    }
}

void
spawnedProcessExit(int32 rc, int32 data UNUSED) {
    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (res) {
        int32 parent = GetPID(0, GPID_PARENT);
        int32 me = GetPID(0, GPID_PROCESS);
        size_t iter = 0;
        void *item;

        while (hashmap_iter(res->children, &iter, &item)) {
            const struct Clib4Node *node = item;

            // DebugPrintF("[spawneeExit :] *** iteration : pid=%d\n", node->pid);

            if (node->pid == parent) {
                struct Clib4Children key;
                key.pid = me;
                struct Clib4Children *item = (struct Clib4Children *) hashmap_get(node->spawnedProcesses, &key);
                if (item != NULL) {
                    // SHOWMSG("[spawneeExit :] SUCCESS");
                    // DebugPrintF("[spawneeExit :] SUCCESS\n");
                    item->returnCode = ~0x80000000 & rc;
                    break;
                }
            }
        }
    }
}