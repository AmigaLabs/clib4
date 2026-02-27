/*
 * $Id: wait_wait.c,v 1.0 2023-06-09 12:04:27 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <sys/wait.h>

#include "clib4.h"

pid_t waitpid(pid_t pid, int *status, int options) {
    // Delay(1);
    uint32 me = GetPID(0, GPID_PROCESS);

    if (options != 0 && options != WNOHANG) {
        __set_errno(EINVAL);
        return -1;
    }

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (!res) {
        *status = 0;
        __set_errno(EINVAL);
        return -1;
    }

    size_t childrenIter = 0;
    void *childItem;
    uint32 pidFound = 0;
    BOOL done = FALSE;

    while (!done && hashmap_iter(res->children, &childrenIter, &childItem)) {
        const struct Clib4Node *node = childItem;
        struct Clib4Children *children;

        if (node->pid != me) continue;

        /* If we have no spawned processes return ECHILD error */
        if (node->spawnedProcesses == NULL || hashmap_count(node->spawnedProcesses) == 0) {
            *status = 0;
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
                __set_errno(ECHILD);
                *status = 0;
                D(("Child with pid %ld not found in spawned process list.\n", pid));
                return -1;
            }

            if (options & WNOHANG) {
                D(("Check for child with pid %ld\n", pid));
                found = CheckForChildExit(pid);

                /* Set returnCode status */
                *status = item->returnCode;
                
                /* If returnCode still has 0x10000000 bit set, the child hasn't finished 
                 * its exit callback yet, so treat it as still running */
                if (*status & 0x10000000) {
                    D(("Child with pid %ld exit callback not finished yet (status=0x%lx), treating as still running\n", pid, *status));
                    found = TRUE;  /* Treat as still running */
                    *status = 0;
                }

                if (!found) hashmap_delete(node->spawnedProcesses, item);

                D(("Child with pid %ld %s with status 0x%lx\n", pid, found ? "was found" : "has exited", *status));
            } else {
                D(("Waiting for child with pid %ld to exit...", pid));
                found = WaitForChildExit(pid);

                /* Set returnCode status */
                *status = item->returnCode;

                hashmap_delete(node->spawnedProcesses, item);

                D(("Child with pid %ld has (to the best of our knowledge) exited with status 0x%lx\n", pid, *status));
            }
            pidFound = pid; done = TRUE;
        } else if (pid == -1) {
            D(("Wait for any child to exit%s\n", options & WNOHANG ? " (WNOHANG)" : ""));
            
            if((options & WNOHANG) == 0) Wait(SIGF_CHILD);

            while (hashmap_iter(node->spawnedProcesses, &iter, &item)) {
                const struct Clib4Children *children = item;

                D(("Check for Child pid %ld\n", children->pid));
                found = CheckForChildExit(children->pid);

                if (!found) {
                    /* Set returnCode on Status */
                    *status = children->returnCode;

                    hashmap_delete(node->spawnedProcesses, item);
                    pidFound = pid;
                    break;
                }
                if (options & WNOHANG) { pidFound = pid; done = TRUE; break; }
            }
        } else { //if (pid < -1 || pid == 0) {
            gid_t gid = (pid == 0 ? getgid() : pid * -1U);

            D(("Searching for children with group id %ld\n", gid));

            // I am just gonna leave it here. Not going to test it. No.
            while (!done) {
                if((options & WNOHANG) == 0) Wait(SIGF_CHILD);
                if (options & WNOHANG) done = TRUE;
                
                while (hashmap_iter(node->spawnedProcesses, &iter, &item)) {
                    const struct Clib4Children *children = item;
                    if (children->groupId != gid) continue;

                    D(("Check for child pid %ld\n", children->pid));
                    found = CheckForChildExit(children->pid);

                    if (!found) {
                        /* Set returnCode on status */
                        *status = children->returnCode;

                        hashmap_delete(node->spawnedProcesses, item);
                        pidFound = pid;
                        done = TRUE;
                        break;
                    }
                    if (options & WNOHANG) { pidFound = pid; done = TRUE; break; }
                }
            }
        }
    }
    return pidFound;
}