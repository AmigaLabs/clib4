/*
 * $Id: wait_wait.c,v 1.0 2023-06-09 12:04:27 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <sys/wait.h>

#include "clib4.h"

static APTR
hook_function(struct Hook *hook, APTR userdata, struct Process *process) {
    uint32 pid = (uint32) userdata;
    (void) (hook);

    if (process->pr_ProcessID == pid) {
        return process;
    }

    return 0;
}

pid_t waitpid(pid_t pid, int *status, int options) {
    Delay(1);
    uint32 me = GetPID(0, GPID_PROCESS);

    if (options != 0 && options != WNOHANG) {
        __set_errno(EINVAL);
        return -1;
    }

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (res) {
        size_t childrenIter = 0;
        void *childItem;

        while (hashmap_iter(res->children, &childrenIter, &childItem)) {
            const struct Clib4Node *node = childItem;
            struct Clib4Children *children;
            if (node->pid == me) {
                /* If we have no spawned processes return ECHILD error */
                if (node->spawnedProcesses == NULL || hashmap_count(node->spawnedProcesses) == 0) {
                    *status = 0;
                    __set_errno(ECHILD);
                    return -1;
                }

                struct Hook h = {{NULL, NULL}, (HOOKFUNC) hook_function, NULL, NULL};
                int32 process;
                size_t iter = 0;
                void *item;
                BOOL found = FALSE;

                if (pid < -1 || pid == 0) {
                    int gid = pid == 0 ? getgid() : pid * -1;
                    D(("Searching for children with group %ld\n", gid));
                    while (hashmap_iter(node->spawnedProcesses, &iter, &item)) {
                        const struct Clib4Children *children = item;
                        if (children->groupId == gid) {
                            found = TRUE;
                            /* Set returnCode on Status */
                            *status = children->returnCode;
                            uint32 pidFound = children->pid;
                            /* Scan for process */
                            D(("Check for Child width pid %ld\n", pidFound));
                            /* Scan for process */
                            process = ProcessScan(&h, (CONST_APTR) pidFound, 0);
                            if (process == 0) {
                                hashmap_delete(node->spawnedProcesses, item);
                                D(("Process with pid %ld not found. Most probably has died\n", pidFound));
                                /* Remove process from spawnedProcess since it is not found anymore */
                                return pidFound;
                            }
                            if (options == 0) {
                                hashmap_delete(node->spawnedProcesses, item);
                                WaitForChildExit(pidFound);
                                return pidFound;
                            } else {
                                int32 found = CheckForChildExit(pidFound);
                                if (!found) {
                                    hashmap_delete(node->spawnedProcesses, item);
                                    D(("Process with pid %ld not found. Most probably has died\n", pidFound));
                                    return pidFound;
                                } else {
                                    D(("Process with pid %ld still running\n", pid));
                                }
                            }
                        }
                    }
                    if (!found) {
                        __set_errno(ECHILD);
                        *status = 0;
                        return -1;
                    }

                    *status = 0xFF;
                    return 0;
                } else if (pid == -1) {
                    D(("Wait for any child\n"));
                    while (hashmap_iter(node->spawnedProcesses, &iter, &item)) {
                        found = TRUE;
                        D(("Found children with pid: %ld\n", pid));
                        const struct Clib4Children *children = item;
                        /* Set returnCode on Status */
                        *status = children->returnCode;
                        uint32 pidFound = children->pid;
                        /* Scan for process */
                        process = ProcessScan(&h, (CONST_APTR) pidFound, 0);
                        if (process > 0) {
                            D(("Children still running\n"));
                            if (options == 0) {
                                hashmap_delete(node->spawnedProcesses, item);
                                WaitForChildExit(pidFound);
                                return pidFound;
                            } else {
                                D(("Check for Child pid %ld\n", pid));
                                int32 found = CheckForChildExit(pidFound);
                                if (!found) {
                                    hashmap_delete(node->spawnedProcesses, item);
                                    D(("Process with pid %ld not found. Most probably has died\n", pidFound));
                                    return pidFound;
                                } else {
                                    D(("Process with pid %ld still running\n", pid));
                                }
                            }
                        } else {
                            D(("Process with pid %ld not found. Most probably has died\n", pidFound));
                            hashmap_delete(node->spawnedProcesses, item);
                            return pidFound;
                        }
                    }
                    if (!found) {
                        __set_errno(ECHILD);
                        *status = 0;
                        return -1;
                    }

                    *status = 0xFF;
                    return 0;
                } else {
                    D(("Searching for children with pid: %ld\n", pid));
                    struct Clib4Children key;
                    key.pid = pid;
                    struct Clib4Children *item = hashmap_get(node->spawnedProcesses, &key);
                    if (item != NULL) {
                        /* Set returnCode on Status */
                        *status = item->returnCode;
                        /* Scan for process */
                        process = ProcessScan(&h, (CONST_APTR) pid, 0);
                        if (process > 0) {
                            D(("Check for Child pid and options %ld\n", pid, options));
                            if (options == 0) {
                                hashmap_delete(node->spawnedProcesses, item);
                                WaitForChildExit(pid);
                                D(("Children with pid %ld has died. Return code: %ld\n", pid, item->returnCode));
                                return pid;
                            } else { // WNOHANG
                                int32 found = CheckForChildExit(item->pid);
                                if (!found) {
                                    hashmap_delete(node->spawnedProcesses, item);
                                    D(("Process with pid %ld not found with CheckForChildExit. Most probably has died. Return code: %ld\n", pid, item->returnCode));
                                    return pid;
                                } else {
                                    D(("Process with pid %ld found with CheckForChildExit and running\n", pid));
                                }
                            }
                        } else {
                            hashmap_delete(node->spawnedProcesses, item);
                            D(("Process with pid %ld not found. Most probably has died. Return code: %ld\n", pid, item->returnCode));
                            return pid;
                        }
                    } else {
                        __set_errno(ECHILD);
                        *status = 0;
                        D(("Children with pid %ld not found!\n", pid));
                        return -1;
                    }
                    *status = 0xFF;
                    return 0;
                }
                break;
            }
        }
    }

    *status = 0;
    __set_errno(EINVAL);
    return -1;
}