/*
 * dump_children.c - Dump della struttura children condivisa
 *
 * Questo programma mostra tutti i processi figli registrati nella
 * struttura Clib4Resource condivisa, utile per debug di problemi con waitpid.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

/* AmigaOS includes */
#include <proto/exec.h>
#include <proto/dos.h>

/* Clib4 internal structures - copied from clib4.h */
#define RESOURCE_NAME "clib4.resource"

struct bucket {
	uint64_t hash: 48;
	uint64_t dib: 16;
};

// hashmap is an open addressed hash map using robinhood hashing.
struct hashmap {
	size_t elsize;
	size_t cap;
	uint64_t seed0;
	uint64_t seed1;
	uint64_t (*hash)(const void *item, uint64_t seed0, uint64_t seed1);
	int (*compare)(const void *a, const void *b, void *udata);
	void (*elfree)(void *item);
	void *udata;
	size_t bucketsz;
	size_t nbuckets;
	size_t count;
	size_t mask;
	size_t growat;
	size_t shrinkat;
	uint8_t growpower;
	bool oom;
	void *buckets;
	void *spare;
	void *edata;
};

/* Copied from library/shared_library/clib4.h */
struct Clib4Children {
    uint32_t pid;        /* the process PID */
    uint32_t groupId;    /* Group ID of process */
    uint32_t returnCode; /* the return code of process */
    FILE     *pipe;
};

struct Clib4Node {
    uint32_t pid;   /* the process PID */
    uint32_t pPid;  /* the process Parent PID */
    char     uuid[37]; /* UUID4_LEN + 1 */
    struct hashmap *spawnedProcesses;
    void *undo;
    int32_t errNo;
};

struct Clib4Resource {
    struct Library resource;           /* must be first */
    uint32_t size;                     /* for struct validation only */
    struct SignalSemaphore semaphore;  /* for list arbitration */
    struct hashmap *children;          /* list of parent nodes */
    /* ... other fields not needed for this test */
};

static struct bucket *bucket_at0(void *buckets, size_t bucketsz, size_t i) {
	return (struct bucket *) (((char *) buckets) + (bucketsz * i));
}

static struct bucket *bucket_at(struct hashmap *map, size_t index) {
	return bucket_at0(map->buckets, map->bucketsz, index);
}

static void *bucket_item(struct bucket *entry) {
	return ((char *) entry) + sizeof(struct bucket);
}

static size_t hashmap_count(struct hashmap *map) {
	return map->count;
}

static bool
hashmap_iter(struct hashmap *map, size_t *i, void **item) {
	struct bucket *bucket;
	do {
		if (*i >= map->nbuckets) return false;
		bucket = bucket_at(map, *i);
		(*i)++;
	} while (!bucket->dib);
	*item = bucket_item(bucket);
	return true;
}

/* Print status flags */
static void print_status_flags(uint32_t status) {
    printf("    Status: 0x%08x\n", status);
    printf("      WIFEXITED: %d", WIFEXITED(status));
    if (WIFEXITED(status)) {
        printf(" (process exited normally)\n");
        printf("      WEXITSTATUS: %d (exit code)\n", WEXITSTATUS(status));
    } else {
        printf(" (process still running or flag 0x10000000 set)\n");
    }
    printf("      WIFSIGNALED: %d", WIFSIGNALED(status));
    if (WIFSIGNALED(status)) {
        printf(" (terminated by signal)\n");
        printf("      WTERMSIG: %d (signal number)\n", WTERMSIG(status));
    } else {
        printf(" (not terminated by signal)\n");
    }
}

/* Check if a process is still running */
static int check_process_exists(pid_t pid) {
    APTR process = (APTR)IDOS->ProcessScan(NULL, (CONST_APTR)(ULONG)pid, 0);
    return (process != NULL);
}

int main(int argc, char *argv[]) {
    struct Clib4Resource *res;
    uint32_t my_pid;
    int verbose = 0;
    int show_all = 0;

    /* Parse arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--all") == 0) {
            show_all = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [-v|--verbose] [-a|--all] [-h|--help]\n", argv[0]);
            printf("\n");
            printf("Dump children processes from Clib4Resource shared structure.\n");
            printf("\n");
            printf("Options:\n");
            printf("  -v, --verbose    Show detailed information\n");
            printf("  -a, --all        Show all parent nodes, not just current process\n");
            printf("  -h, --help       Show this help message\n");
            printf("\n");
            printf("This tool is useful for debugging waitpid issues and\n");
            printf("checking for stale entries in the children hashmap.\n");
            return 0;
        }
    }

    /* Get current process ID */
    my_pid = IDOS->GetPID(0, GPID_PROCESS);

    printf("=== Clib4 Children Dump ===\n");
    printf("Current PID: %u\n", my_pid);
    printf("Current PPID: %u\n", IDOS->GetPID(0, GPID_PARENT));
    printf("\n");

    /* Open the shared resource */
    res = (struct Clib4Resource *)IExec->OpenResource(RESOURCE_NAME);
    if (!res) {
        fprintf(stderr, "ERROR: Cannot open %s\n", RESOURCE_NAME);
        fprintf(stderr, "The clib4.library might not be loaded.\n");
        return 1;
    }

    printf("Clib4Resource found at: %p\n", res);
    printf("Resource size: %u bytes\n", res->size);
    printf("\n");

    /* Acquire semaphore for reading */
    IExec->ObtainSemaphoreShared(&res->semaphore);

    if (!res->children) {
        printf("No children hashmap found!\n");
        IExec->ReleaseSemaphore(&res->semaphore);
        return 0;
    }

    size_t total_nodes = hashmap_count(res->children);
    printf("Total parent nodes in hashmap: %zu\n", total_nodes);
    printf("\n");

    /* Iterate through all parent nodes */
    size_t iter = 0;
    void *item;
    int node_count = 0;
    int total_children = 0;

    while (hashmap_iter(res->children, &iter, &item)) {
        struct Clib4Node *node = (struct Clib4Node *)item;
        node_count++;

        /* Skip other processes if not showing all */
        if (!show_all && node->pid != my_pid) {
            continue;
        }

        printf("---------------------------------------------------------\n");
        printf("Parent Node #%d:\n", node_count);
        printf("  PID: %u", node->pid);
        if (node->pid == my_pid) {
            printf(" (THIS PROCESS)");
        }
        printf("\n");
        printf("  PPID: %u\n", node->pPid);
        printf("  UUID: %s\n", node->uuid);

        if (verbose) {
            int parent_exists = check_process_exists(node->pid);
            printf("  Process exists: %s\n", parent_exists ? "YES" : "NO (ZOMBIE/STALE)");
        }

        if (!node->spawnedProcesses) {
            printf("  No spawned processes hashmap\n");
            continue;
        }

        size_t child_count = hashmap_count(node->spawnedProcesses);
        printf("  Spawned processes: %zu\n", child_count);

        if (child_count == 0) {
            printf("  (empty)\n");
            continue;
        }

        printf("\n");

        /* Iterate through children */
        size_t child_iter = 0;
        void *child_item;
        int child_num = 0;

        while (hashmap_iter(node->spawnedProcesses, &child_iter, &child_item)) {
            struct Clib4Children *child = (struct Clib4Children *)child_item;
            child_num++;
            total_children++;

            printf("  Child #%d:\n", child_num);
            printf("    PID: %u\n", child->pid);
            printf("    Group ID: %u\n", child->groupId);
            printf("    Return Code: 0x%08x (%d)\n", child->returnCode, child->returnCode);

            if (verbose) {
                print_status_flags(child->returnCode);

                int exists = check_process_exists(child->pid);
                printf("    Process exists: %s\n", exists ? "YES" : "NO");

                if (!exists && (child->returnCode & 0x10000000)) {
                    printf("    *** WARNING: Process doesn't exist but flag 0x10000000 is set!\n");
                    printf("    *** This might be a stale entry that wasn't cleaned up.\n");
                }

                if (exists && !(child->returnCode & 0x10000000)) {
                    printf("    *** WARNING: Process still exists but flag 0x10000000 is NOT set!\n");
                    printf("    *** Process might have exited but entry not updated.\n");
                }
            }

            if (child->pipe) {
                printf("    Pipe handle: %p\n", child->pipe);
            }

            printf("\n");
        }
    }

    IExec->ReleaseSemaphore(&res->semaphore);

    printf("---------------------------------------------------------\n");
    printf("Summary:\n");
    printf("  Parent nodes: %d\n", node_count);
    printf("  Total children: %d\n", total_children);
    printf("\n");

    if (!verbose) {
        printf("Use -v for detailed information about each child.\n");
    }

    if (!show_all && node_count == 0) {
        printf("\nNo children found for current process (PID %u).\n", my_pid);
        printf("Use -a to show all parent nodes.\n");
    }

    return 0;
}

