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

    if (options != 0 && options != WNOHANG) {
        __set_errno(EINVAL);
        return -1;
    }

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (res) {
        struct Hook h = {{NULL, NULL}, (HOOKFUNC) hook_function, NULL, NULL};
        int32 process;
        size_t iter = 0;
        void *item;

        if (pid < -1 || pid == 0) {
            int gid = pid == 0 ? getgid() : pid * -1;
            D(("Searching for children with group %ld\n", gid));
            while (hashmap_iter(res->spawnedProcesses, &iter, &item)) {
                const struct Clib4Children *children = item;
                if (children->groupId == gid) {
                    D(("Check for Child width pid %ld\n", children->pid));
                    process = ProcessScan(&h, (CONST_APTR) children->pid, 0);
                    if (process == 0) {
                        D(("Process with pid %ld not found. Most probably has died\n", children->pid));
                        /* Remove process from spawnedProcess since it is not found anymore */
                        *status = 0;
                        hashmap_delete(res->spawnedProcesses, item);
                        return children->pid;
                    }
                    if (options == 0) {
                        WaitForChildExit(children->pid);
                        *status = 0;
                        hashmap_delete(res->spawnedProcesses, item);
                        return children->pid;
                    }
                    else {
                        int32 found = CheckForChildExit(children->pid);
                        if (!found) {
                            D(("Process with pid %ld not found. Most probably has died\n", children->pid));
                            *status = 0;
                            hashmap_delete(res->spawnedProcesses, item);
                            return children->pid;
                        }
                        else {
                            D(("Process with pid %ld still running\n", pid));
                        }
                    }
                }
            }
            *status = 0xFF;
            return 0;
        } else if (pid == -1) {
            D(("Searching for any children\n"));
            while (hashmap_iter(res->spawnedProcesses, &iter, &item)) {
                D(("Found children with pid: %ld\n", pid));
                const struct Clib4Children *children = item;
                process = ProcessScan(&h, (CONST_APTR) children->pid, 0);
                if (process > 0) {
                    D(("Children still running\n"));
                    if (options == 0) {
                        WaitForChildExit(children->pid);
                        *status = 0;
                        hashmap_delete(res->spawnedProcesses, item);
                        return children->pid;
                    }
                    else {
                        D(("Check for Child pid %ld\n", pid));
                        int32 found = CheckForChildExit(children->pid);
                        if (!found) {
                            D(("Process with pid %ld not found. Most probably has died\n", children->pid));
                            *status = 0;
                            hashmap_delete(res->spawnedProcesses, item);
                            return children->pid;
                        }
                        else {
                            D(("Process with pid %ld still running\n", pid));
                        }
                    }
                }
                else {
                    *status = 0;
                    hashmap_delete(res->spawnedProcesses, item);
                    return children->pid;
                }
            }
            *status = 0xFF;
            return 0;
        } else {
            D(("Searching for children with pid: %ld\n", pid));
            struct Clib4Children key;
            key.pid = pid;
            struct Clib4Children *item = hashmap_get(res->spawnedProcesses, &key);
            if (item != NULL) {
                /* Scan for process */
                process = ProcessScan(&h, (CONST_APTR) pid, 0);
                D(("Children with pid %ld and process %ld found\n", pid, process));
                if (process > 0) {
                    if (options == 0) {
                        D(("Wait on Child pid %ld\n", pid));
                        WaitForChildExit(pid);
                        D(("Children with pid %ld has died\n", pid));
                        *status = 0;
                        hashmap_delete(res->spawnedProcesses, item);
                        return pid;
                    }
                    else {
                        D(("Check for Child pid %ld\n", pid));
                        int32 found = CheckForChildExit(item->pid);
                        if (!found) {
                            D(("Process with pid %ld not found. Most probably has died\n", pid));
                            *status = 0;
                            hashmap_delete(res->spawnedProcesses, item);
                            return item->pid;
                        }
                        else {
                            D(("Process with pid %ld still running\n", pid));
                        }
                    }
                }
                else {
                    D(("Process with pid %ld not found. Most probably has died\n", pid));
                    *status = 0;
                    hashmap_delete(res->spawnedProcesses, item);
                    return pid;
                }
            }
            else {
                *status = 0;
                D(("Childred with pid %ld not found!\n", pid));
                return item->pid;
            }
            *status = 0xFF;
            return 0;
        }
    }
    else {
        __set_errno(EINVAL);
        return -1;
    }
}