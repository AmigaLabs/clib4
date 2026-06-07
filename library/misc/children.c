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

void
import_inherited_fds_from_spec(struct _clib4 *__clib4, const char *spec) {
    char *copy;
    char *entry;

    if (spec == NULL || spec[0] == '\0')
        return;

    copy = strdup(spec);
    if (copy == NULL)
        return;

    entry = strtok(copy, ";");
    while (entry != NULL) {
        char *fd_str = entry;
        char *flags_str = strchr(fd_str, ':');

        if (flags_str != NULL) {
            *flags_str++ = '\0';
            char *handle_str = strchr(flags_str, ':');
            if (handle_str != NULL) {
                *handle_str++ = '\0';

                int fd_num = (int) strtol(fd_str, NULL, 10);
                ULONG flags = (ULONG) strtoul(flags_str, NULL, 10);
                BPTR inherited_handle = BZERO;
                BOOL no_close = FALSE;

                /*
                 * Symbolic tokens: the parent detected that this fd shares
                 * the same underlying pipe handle as fhin/fhout/fherr.
                 * AmigaOS4's PIPE: device does not properly share the data
                 * stream with DupFileHandle copies, so we map these fds to
                 * the process-level Input()/Output()/ErrorOutput() handles
                 * which are guaranteed to be connected to the right pipe.
                 * Mark FDF_NO_CLOSE_BPTR so clib4 doesn't close these
                 * process-managed handles.
                 */
                if (strcmp(handle_str, "STDIN") == 0) {
                    inherited_handle = Input();
                    no_close = TRUE;
                } else if (strcmp(handle_str, "STDOUT") == 0) {
                    inherited_handle = Output();
                    no_close = TRUE;
                } else if (strcmp(handle_str, "STDERR") == 0) {
                    inherited_handle = ErrorOutput();
                    no_close = TRUE;
                } else {
                    inherited_handle = (BPTR) strtoul(handle_str, NULL, 16);
                }

                if (fd_num > STDERR_FILENO && inherited_handle != BZERO) {
                    if (fd_num >= __clib4->__num_fd) {
                        if (__grow_fd_table(__clib4, fd_num + 1) < 0) {
                            entry = strtok(NULL, ";");
                            continue;
                        }
                    }

                    APTR lock = __create_mutex();
                    if (lock != NULL) {
                        ULONG inherited_flags = flags;
                        CLEAR_FLAG(inherited_flags, FDF_CLOEXEC);
                        SET_FLAG(inherited_flags, FDF_IN_USE);
                        if (no_close)
                            SET_FLAG(inherited_flags, FDF_NO_CLOSE_BPTR);

                        __initialize_fd(__clib4->__fd[fd_num],
                                        __fd_hook_entry,
                                        inherited_handle,
                                        inherited_flags,
                                        lock);
                    }
                }
            }
        }

        entry = strtok(NULL, ";");
    }

    free(copy);
}

static void *
gidChildrenScan(const void *children, void *gid) {
    const struct Clib4Children *myChildren = children;
    const gid_t groupId = *((gid_t *)gid);
    if (myChildren->groupId == groupId)
        return (struct Clib4Children *) children;
    return NULL;
}

static void *
pidChildrenScan(const void *children, void *pid) {
    const struct Clib4Children *myChildren = children;
    const uint32 processId = *((uint32 *)pid);
    if (myChildren->pid == (uint32) processId)
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
insertSpawnedChildren(uint32 pid, uint32 gid, const char *parentUuid, const char *fdInherit) {
    DECLARE_UTILITYBASE();

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (res) {
        struct Clib4Children children;
        children.pid = pid;
        children.returnCode = 0x10000000; //set this flag for WIFEXITED
        children.groupId = gid;
        children.pipe = NULL;
        /* take direct ownership of the heap-allocated spec; caller must set its ptr to NULL */
        children.fdInherit = (fdInherit != NULL && fdInherit[0] != '\0') ? (char *)fdInherit : NULL;

        /* Use direct hashmap_get by uuid — avoids hashmap_iter race condition */
        struct Clib4Node nodeKey;
        memset(&nodeKey, 0, sizeof(nodeKey));
        strncpy(nodeKey.uuid, parentUuid, UUID4_LEN);
        struct Clib4Node *node = (struct Clib4Node *) hashmap_get(res->children, &nodeKey);
        if (node) {
            hashmap_set(node->spawnedProcesses, &children);
            D(("Inserted child pid %ld into parent uuid %s\n", pid, parentUuid));
            return TRUE;
        }
        D(("Parent uuid %s not found in res->children\n", parentUuid));
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
	struct spawnData *data = (struct spawnData *) entry_data;
    gid_t groupId = data->groupId;
	struct Task *parentTask = data->parentTask;

    uint32 pid = GetPID(0, GPID_PROCESS);
    /* fdInherit ownership transferred into Clib4Children; consumed later in libOpen */
    if (insertSpawnedChildren(pid, groupId, data->parentUuid, data->fdInherit)) {
        data->fdInherit = NULL;  /* ownership transferred */
        __CLIB4->__children++;
        D(("Children with pid %ld and gid %ld inserted into list\n", pid, groupId));
    }
    else {
        D(("Cannot insert children with pid %ld and gid %ld into list\n", pid, groupId));
        /* insertion failed: free the spec that was not transferred */
        free(data->fdInherit);
        data->fdInherit = NULL;
    }
	free(data);
	Signal(parentTask, SIGF_CHILD);
}

void
import_pending_fds_for_process(struct _clib4 *__clib4, uint32 pid, uint32 ppid) {
    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (!res) return;

    size_t iter = 0;
    void *item;
    while (hashmap_iter(res->children, &iter, &item)) {
        struct Clib4Node *node = item;
        if (node->pid == ppid) {
            struct Clib4Children key;
            memset(&key, 0, sizeof(key));
            key.pid = pid;
            struct Clib4Children *ce = (struct Clib4Children *) hashmap_get(node->spawnedProcesses, &key);
            if (ce != NULL && ce->fdInherit != NULL) {
                import_inherited_fds_from_spec(__clib4, ce->fdInherit);
                free(ce->fdInherit);
                ce->fdInherit = NULL;
            }
            break;
        }
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

            if (node->pid == (uint32) parent) {
                struct Clib4Children key;
                key.pid = me;
                struct Clib4Children *item = (struct Clib4Children *) hashmap_get(node->spawnedProcesses, &key);
                if (item != NULL) {
                    D(("Found child with pid %ld of parent %ld that exits now with return code %ld\n", me, parent, rc));
                    item->returnCode = ~0x10000000 & rc;
                    break;
                }
            }
        }
    }
}