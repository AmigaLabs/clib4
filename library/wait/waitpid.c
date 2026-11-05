/*
 * $Id: wait_wait.c,v 1.0 2023-06-09 12:04:27 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <sys/wait.h>

#include "clib4.h"

/* Release the fd-inherit spec of a child entry about to be removed from
 * the spawnedProcesses hashmap. The spec is AllocVecTags'd by the parent
 * in build_fd_inherit_spec and normally consumed (and freed) by the child
 * in import_pending_fds_for_process, but a child that is not a clib4
 * program never consumes it. */
static void
free_child_fd_inherit(void *item) {
    struct Clib4Children *child = item;
    if (child != NULL && child->fdInherit != NULL) {
        FreeVec(child->fdInherit);
        child->fdInherit = NULL;
    }
}

/* dos.library 53.x does not implement WaitForChildExit()/CheckForChildExit():
 * their interface slots are a stub that raises an AG_Obsolete|AO_DOSLib
 * alert (Guru 000C8007) and suspends the caller. Without them we rely on our
 * own bookkeeping: spawnedProcessExit() clears the 0x10000000 bit of
 * returnCode when the child exits. */
static BOOL
dos_has_child_exit(void) {
    return ((struct Library *) IDOS->Data.LibBase)->lib_Version >= 54;
}

/* TRUE while the child is still running (CheckForChildExit() semantics). */
static BOOL
child_running(const struct Clib4Children *child) {
    if (child->returnCode & 0x10000000)
        return TRUE;  /* exit callback not finished yet */
    return dos_has_child_exit() ? CheckForChildExit(child->pid) : FALSE;
}

/* This program's node, with the maps locked by the caller. The node may move once they are not. */
static const struct Clib4Node *
own_node(struct Clib4Resource *res, const char *uuid) {
    struct Clib4Node nodeKey;

    memset(&nodeKey, 0, sizeof(nodeKey));
    strncpy(nodeKey.uuid, uuid, UUID4_LEN);
    return hashmap_get(res->children, &nodeKey);
}

/* A child's entry, with the maps locked by the caller. */
static struct Clib4Children *
own_child(struct Clib4Resource *res, const char *uuid, pid_t pid) {
    const struct Clib4Node *node = own_node(res, uuid);
    struct Clib4Children key;

    if (node == NULL || node->spawnedProcesses == NULL)
        return NULL;
    key.pid = pid;
    return hashmap_get(node->spawnedProcesses, &key);
}

/* The bit that says the exit callback has not finished: looked up anew each time, as the entry can move. */
static BOOL
child_not_ended(struct Clib4Resource *res, const char *uuid, pid_t pid) {
    const struct Clib4Children *child;
    BOOL running;

    ObtainSemaphore(&res->mapLock);
    child = own_child(res, uuid, pid);
    running = child != NULL && (child->returnCode & 0x10000000) != 0;
    ReleaseSemaphore(&res->mapLock);
    return running;
}

static void
wait_child_exit(struct Clib4Resource *res, const char *uuid, pid_t pid) {
    if (dos_has_child_exit()) {
        WaitForChildExit(pid);

        /* WaitForChildExit may return before NP_ExitCode (spawnedProcessExit)
         * has cleared the 0x10000000 flag in returnCode. Wait briefly. */
        int retries = 50; /* up to ~1 second */
        while (child_not_ended(res, uuid, pid) && retries-- > 0)
            Delay(1);
    } else {
        while (child_not_ended(res, uuid, pid))
            Delay(1);
    }
}

/* Its status, and the entry goes, with the maps locked by the caller. */
static void
take_child(const struct Clib4Node *node, struct Clib4Children *child, int *status) {
    if (status) *status = child->returnCode;
    free_child_fd_inherit(child);
    hashmap_delete(node->spawnedProcesses, child);
}

pid_t waitpid(pid_t pid, int *status, int options) {
    struct _clib4 *__clib4 = __CLIB4;
    uint32 me = GetPID(0, GPID_PROCESS);

    if (options != 0 && options != WNOHANG) {
        __set_errno(EINVAL);
        return -1;
    }

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (!res) {
        if (status) *status = 0;
        __set_errno(EINVAL);
        return -1;
    }

    /* Use direct hashmap_get with uuid key instead of iterating.
     * The maps are locked only to read them: never across a wait, and whatever was
     * found is looked up again after one. */
    uint32 pidFound = 0;
    BOOL done = FALSE;
    BOOL noNode, noChildren;

    ObtainSemaphore(&res->mapLock);
    {
        const struct Clib4Node *node = own_node(res, __clib4->uuid);

        noNode = node == NULL;
        noChildren = node != NULL && (node->spawnedProcesses == NULL || hashmap_count(node->spawnedProcesses) == 0);
    }
    ReleaseSemaphore(&res->mapLock);

    if (noNode) {
        /* Our own node was not found — should not happen */
        if (status) *status = 0;
        __set_errno(ECHILD);
        return -1;
    }

    /* If we have no spawned processes return ECHILD error */
    if (noChildren) {
        if (status) *status = 0;
        __set_errno(ECHILD);
        return -1;
    }

    size_t iter = 0;
    void *item;
    int32 found = FALSE;

    if (pid > 0) {
        BOOL known;

        D(("Searching for child with pid %ld%s\n", pid, options & WNOHANG ? " (WNOHANG)" : ""));

        ObtainSemaphore(&res->mapLock);
        known = own_child(res, __clib4->uuid, pid) != NULL;
        ReleaseSemaphore(&res->mapLock);

        if (!known) {
            /* In blocking mode, the child's NP_EntryCode may not have run yet.
             * Give it a moment to register, then retry once. */
            if (!(options & WNOHANG)) {
                Delay(1);
                ObtainSemaphore(&res->mapLock);
                known = own_child(res, __clib4->uuid, pid) != NULL;
                ReleaseSemaphore(&res->mapLock);
            }
        }

        if (!known) {
            __set_errno(ECHILD);
            if (status) *status = 0;
            D(("Child with pid %ld not found in spawned process list.\n", pid));
            return -1;
        }

        if (options & WNOHANG) {
            D(("Check for child with pid %ld\n", pid));
            ObtainSemaphore(&res->mapLock);
            {
                struct Clib4Children *child = own_child(res, __clib4->uuid, pid);

                found = child != NULL && child_running(child);
                if (found) {
                    /* POSIX: WNOHANG on a child that is still running returns 0 */
                    D(("Child with pid %ld is still running\n", pid));
                    if (status) *status = 0;
                    ReleaseSemaphore(&res->mapLock);
                    return 0;
                }
                if (child != NULL)
                    take_child(own_node(res, __clib4->uuid), child, status);
            }
            ReleaseSemaphore(&res->mapLock);

            D(("Child with pid %ld has exited with status 0x%lx\n", pid, status ? *status : 0));
        } else {
            D(("Waiting for child with pid %ld to exit...", pid));
            wait_child_exit(res, __clib4->uuid, pid);

            /* Set returnCode status */
            ObtainSemaphore(&res->mapLock);
            {
                struct Clib4Children *child = own_child(res, __clib4->uuid, pid);

                if (child != NULL)
                    take_child(own_node(res, __clib4->uuid), child, status);
            }
            ReleaseSemaphore(&res->mapLock);

            D(("Child with pid %ld has (to the best of our knowledge) exited with status 0x%lx\n", pid, status ? *status : 0));
        }
        pidFound = pid; done = TRUE;
    } else if (pid == -1) {
        D(("Wait for any child to exit%s\n", options & WNOHANG ? " (WNOHANG)" : ""));

        /* Poll: children created with spawnvpe_fork() do not signal
         * SIGF_CHILD to us when they exit. */
        while (1) {
            BOOL none;

            ObtainSemaphore(&res->mapLock);
            const struct Clib4Node *node = own_node(res, __clib4->uuid);

            none = node == NULL || node->spawnedProcesses == NULL;
            if (!none) {
                iter = 0;
                while (hashmap_iter(node->spawnedProcesses, &iter, &item)) {
                    struct Clib4Children *children = item;

                    D(("Check for Child pid %ld\n", children->pid));
                    found = child_running(children);

                    if (!found) {
                        /* Set returnCode on Status */
                        pidFound = children->pid;
                        take_child(node, children, status);
                        break;
                    }
                }
                none = hashmap_count(node->spawnedProcesses) == 0;
            }
            ReleaseSemaphore(&res->mapLock);

            if (pidFound != 0 || (options & WNOHANG) || none)
                break;
            Delay(1);
        }
        /* POSIX: WNOHANG with no exited child returns 0 (pidFound is still 0) */
        if (pidFound == 0 && status) *status = 0;
    } else { //if (pid < -1 || pid == 0) {
        gid_t gid = (pid == 0 ? getgid() : pid * -1U);

        D(("Searching for children with group id %ld\n", gid));

        // I am just gonna leave it here. Not going to test it. No.
        while (!done) {
            if (options & WNOHANG) done = TRUE;

            ObtainSemaphore(&res->mapLock);
            const struct Clib4Node *node = own_node(res, __clib4->uuid);

            iter = 0;
            while (node != NULL && node->spawnedProcesses != NULL && hashmap_iter(node->spawnedProcesses, &iter, &item)) {
                struct Clib4Children *children = item;
                if (children->groupId != gid) continue;

                D(("Check for child pid %ld\n", children->pid));
                found = child_running(children);

                if (!found) {
                    /* Set returnCode on status */
                    pidFound = children->pid;
                    take_child(node, children, status);
                    done = TRUE;
                    break;
                }
            }
            ReleaseSemaphore(&res->mapLock);

            if (!done)
                Delay(1);  /* poll, see the pid == -1 case */
        }
        /* POSIX: WNOHANG with no exited child returns 0 (pidFound is still 0) */
        if (pidFound == 0 && status) *status = 0;
    }
    return pidFound;
}