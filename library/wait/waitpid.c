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

static void
wait_child_exit(const struct Clib4Children *child) {
    if (dos_has_child_exit()) {
        WaitForChildExit(child->pid);

        /* WaitForChildExit may return before NP_ExitCode (spawnedProcessExit)
         * has cleared the 0x10000000 flag in returnCode. Wait briefly. */
        int retries = 50; /* up to ~1 second */
        while ((child->returnCode & 0x10000000) && retries-- > 0)
            Delay(1);
    } else {
        while (child->returnCode & 0x10000000)
            Delay(1);
    }
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
     * hashmap_iter can miss entries when the hashmap is concurrently modified
     * by libOpen/libClose in other processes. */
    struct Clib4Node nodeKey;
    memset(&nodeKey, 0, sizeof(nodeKey));
    strncpy(nodeKey.uuid, __clib4->uuid, UUID4_LEN);
    const struct Clib4Node *node = hashmap_get(res->children, &nodeKey);

    uint32 pidFound = 0;
    BOOL done = FALSE;

    if (node != NULL) {

        /* If we have no spawned processes return ECHILD error */
        if (node->spawnedProcesses == NULL || hashmap_count(node->spawnedProcesses) == 0) {
            if (status) *status = 0;
            __set_errno(ECHILD);
            return -1;
        }

        size_t iter = 0;
        void *item;
        int32 found = FALSE;

        if (pid > 0) {
            D(("Searching for child with pid %ld%s\n", pid, options & WNOHANG ? " (WNOHANG)" : ""));

            struct Clib4Children key;
            key.pid = pid;
            struct Clib4Children *item = hashmap_get(node->spawnedProcesses, &key);

            if (item == NULL) {
                /* In blocking mode, the child's NP_EntryCode may not have run yet.
                 * Give it a moment to register, then retry once. */
                if (!(options & WNOHANG)) {
                    Delay(1);
                    item = hashmap_get(node->spawnedProcesses, &key);
                }
            }

            if (item == NULL) {
                __set_errno(ECHILD);
                if (status) *status = 0;
                D(("Child with pid %ld not found in spawned process list.\n", pid));
                return -1;
            }

            if (options & WNOHANG) {
                D(("Check for child with pid %ld\n", pid));
                found = child_running(item);

                if (found) {
                    /* POSIX: WNOHANG on a child that is still running returns 0 */
                    D(("Child with pid %ld is still running\n", pid));
                    if (status) *status = 0;
                    return 0;
                }

                if (status) *status = item->returnCode;
                free_child_fd_inherit(item);
                hashmap_delete(node->spawnedProcesses, item);

                D(("Child with pid %ld has exited with status 0x%lx\n", pid, status ? *status : 0));
            } else {
                D(("Waiting for child with pid %ld to exit...", pid));
                wait_child_exit(item);

                /* Set returnCode status */
                if (status) *status = item->returnCode;

                free_child_fd_inherit(item);
                hashmap_delete(node->spawnedProcesses, item);

                D(("Child with pid %ld has (to the best of our knowledge) exited with status 0x%lx\n", pid, status ? *status : 0));
            }
            pidFound = pid; done = TRUE;
        } else if (pid == -1) {
            D(("Wait for any child to exit%s\n", options & WNOHANG ? " (WNOHANG)" : ""));
            
            /* Poll: children created with spawnvpe_fork() do not signal
             * SIGF_CHILD to us when they exit. */
            while (1) {
                iter = 0;
                while (hashmap_iter(node->spawnedProcesses, &iter, &item)) {
                    const struct Clib4Children *children = item;

                    D(("Check for Child pid %ld\n", children->pid));
                    found = child_running(children);

                    if (!found) {
                        /* Set returnCode on Status */
                        if (status) *status = children->returnCode;

                        pidFound = children->pid;
                        free_child_fd_inherit(item);
                        hashmap_delete(node->spawnedProcesses, item);
                        break;
                    }
                }
                if (pidFound != 0 || (options & WNOHANG) || hashmap_count(node->spawnedProcesses) == 0)
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

                iter = 0;
                while (hashmap_iter(node->spawnedProcesses, &iter, &item)) {
                    const struct Clib4Children *children = item;
                    if (children->groupId != gid) continue;

                    D(("Check for child pid %ld\n", children->pid));
                    found = child_running(children);

                    if (!found) {
                        /* Set returnCode on status */
                        if (status) *status = children->returnCode;

                        pidFound = children->pid;
                        free_child_fd_inherit(item);
                        hashmap_delete(node->spawnedProcesses, item);
                        done = TRUE;
                        break;
                    }
                }
                if (!done)
                    Delay(1);  /* poll, see the pid == -1 case */
            }
            /* POSIX: WNOHANG with no exited child returns 0 (pidFound is still 0) */
            if (pidFound == 0 && status) *status = 0;
        }
    } else {
        /* Our own node was not found — should not happen */
        if (status) *status = 0;
        __set_errno(ECHILD);
        return -1;
    }
    return pidFound;
}