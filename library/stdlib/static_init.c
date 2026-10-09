/*
 * static_init.c - clib4 setup and teardown for a statically linked program.
 *
 * shared_library/clib4.c does this work for the shared library. libInit() creates
 * clib4.resource, the state every process shares, the first time the library
 * opens. libOpen() and libClose() set up and tear down each process. A static
 * program has no library to open, so crtbegin.c calls static_clib4_init() and
 * static_clib4_exit() instead. The functions below are copies of the ones in
 * shared_library/clib4.c, kept in the same order. That file is not part of the
 * static archive, and the PowerPC object is left as it is, so the code is
 * duplicated for now.
 *
 * Only the ARM static libc is built from this file (see libc.gmk).
 */

/* As in shared_library/clib4.c: no global interface pointers from the SDK
 * headers and no inline wrappers. The globals are the ones crtbegin.c defines. */
#define __NOLIBBASE__
#define __NOGLOBALIFACE__
#undef __USE_INLINE__

#include "stdlib_headers.h"
#include "stdio_headers.h"
#include "unistd_headers.h"
#include "time_headers.h"
#include "socket_headers.h"
#include "stdlib_utilitybase.h"
#include "stdlib_protos.h"
#include "stdlib_constructor.h"
#include "../c.lib_rev.h"
#include "../misc/map.h"
#include "../posix/ipc_headers.h"
#include "../wmem/wmem_core.h"

#include "../shared_library/clib4.h"

extern struct ExecIFace *IExec;    /* crtbegin.c */
extern struct DOSIFace *IDOS;      /* crtbegin.c */

#define ENVBUF        256
#define ENVIRON_SIZE  4096

extern void import_pending_fds_for_process(struct _clib4 *__clib4, uint32 pid, uint32 ppid);

struct envHookData {
    uint32_t env_size;
    uint32_t allocated_size;
    struct _clib4 *r;
};

static char *empty_env[1] = {NULL};

/* Same order as clib4_init() in shared_library/clib4.c. */
static void
clib4_init(void) {
    stdlib_memory_init();          /* STDLIB - memory allocator */
    stdio_init();                  /* STDIO */
    stdio_file_init();             /* FILE - standard I/O streams */
    math_init();                   /* MATH */
    socket_init();                 /* SOCKET */
    locale_init();                 /* CLIB */
    usergroup_init();              /* CLIB */
    timezone_init();               /* CLIB */
    unistd_init();                 /* CLIB */
    timer_init();                  /* CLIB - must run before clock_init (gettimeofday) */
    clock_init();                  /* CLIB */
    dirent_init();                 /* CLIB */
}

/* Same order as clib4_exit() in shared_library/clib4.c. */
static void
clib4_exit(void) {
    dirent_exit();                 /* CLIB */
    timer_exit();                  /* CLIB */
    unistd_exit();                 /* CLIB */
    __wildcard_expand_exit();      /* CLIB */
    __chdir_exit();                /* CLIB */
    __setenv_exit();               /* CLIB */
    dcngettext_exit();             /* CLIB */
    timezone_exit();               /* CLIB */
    usergroup_exit();              /* CLIB */
    locale_exit();                 /* CLIB */
    socket_exit();                 /* SOCKET */
    stdio_exit();                  /* STDIO */
    stdlib_memory_exit();          /* STDLIB - memory allocator, always last */
}

/* clib4.resource. shared_library/clib4.c (libInit) builds it the first time the
 * library opens; a static program builds it when it is not there yet. Nothing
 * removes it afterwards: it outlives the program that made it, as the library's
 * resource does. The children of the spawning process are not recorded here. */

static uint64_t
staticUnixSocketHash(const void *item, uint64_t seed0, uint64_t seed1) {
    const struct UnixSocket *unixSocket = item;
    return hashmap_xxhash3(unixSocket->name, strlen(unixSocket->name), seed0, seed1);
}

static int
staticUnixSocketCompare(const void *a, const void *b, void *udata) {
    const struct UnixSocket *ua = a;
    const struct UnixSocket *ub = b;
    return strcmp(ua->name, ub->name);
}

static uint64_t
staticClib4NodeHash(const void *item, uint64_t seed0, uint64_t seed1) {
    const struct Clib4Node *node = item;
    return hashmap_xxhash3(node->uuid, strlen(node->uuid), seed0, seed1);
}

static int
staticClib4NodeCompare(const void *a, const void *b, void *udata) {
    const struct Clib4Node *ua = a;
    const struct Clib4Node *ub = b;
    return strcmp(ua->uuid, ub->uuid);
}

/* As IPCMapInit() in posix/sysv_idkey.c. That file stays off ARM for now (its
 * WakeList() waits with Reschedule(), which the ARM SDK does not declare). */
static void
static_ipc_map_init(struct IPCIdKeyMap *m) {
    if (!m->Lock) {
        m->nobj = 0;
        m->vlen = 0;
        m->idx = 0;
        m->nused = 0;
        m->objv = 0;
        m->Lock = IExec->AllocSysObjectTags(ASOT_SEMAPHORE, TAG_DONE);
    }
}

/* Builds a new resource, not yet published. NULL on failure. */
static struct Clib4Resource *
static_clib4_resource_new(void) {
    struct Clib4Resource *res;

    res = (struct Clib4Resource *) IExec->AllocVecTags(sizeof(struct Clib4Resource),
                                                       AVT_Type, MEMF_SHARED,
                                                       AVT_ClearWithValue, 0,
                                                       AVT_Lock, TRUE,
                                                       TAG_END);
    if (res == NULL)
        return NULL;

    res->resource.lib_Version = VERSION;
    res->resource.lib_Revision = REVISION;
    res->resource.lib_IdString = (STRPTR) RESOURCE_NAME;
    res->resource.lib_Node.ln_Name = (STRPTR) RESOURCE_NAME;
    res->resource.lib_Node.ln_Type = NT_RESOURCE;
    IExec->InitSemaphore(&res->semaphore);

    res->children = hashmap_new(sizeof(struct Clib4Node), 0, 0, 0, staticClib4NodeHash, staticClib4NodeCompare, NULL, NULL);
    res->uxSocketsMap = hashmap_new(sizeof(struct UnixSocket), 0, 0, 0, staticUnixSocketHash, staticUnixSocketCompare, NULL, NULL);

    res->fallbackClib = (struct _clib4 *) IExec->AllocVecTags(sizeof(struct _clib4),
                                                              AVT_Type, MEMF_SHARED,
                                                              AVT_ClearWithValue, 0,
                                                              TAG_DONE);
    if (res->children == NULL || res->uxSocketsMap == NULL ||
        res->fallbackClib == NULL || !reent_init(res->fallbackClib, TRUE)) {
        if (res->fallbackClib != NULL)
            IExec->FreeVec(res->fallbackClib);
        if (res->children != NULL)
            hashmap_free(res->children);
        if (res->uxSocketsMap != NULL)
            hashmap_free(res->uxSocketsMap);
        IExec->FreeVec(res);
        return NULL;
    }
    res->fallbackClib->self = (struct Process *) IExec->FindTask(NULL);
    res->fallbackClib->__check_abort_enabled = TRUE;
    res->fallbackClib->__fully_initialized = TRUE;

    /* SysV IPC tables; the limits are the library's defaults. */
    static_ipc_map_init(&res->shmcx.keymap);
    res->shmcx.totshm = 0;
    res->shmcx.shmmax = DEF_SHMMAX;
    res->msgcx.qsizemax = DEF_QSIZEMAX;
    static_ipc_map_init(&res->msgcx.keymap);
    static_ipc_map_init(&res->semcx.keymap);

    res->size = sizeof(*res);
    return res;
}

/* Returns the shared resource, creating it if no program has yet. */
static struct Clib4Resource *
static_clib4_resource(void) {
    struct Clib4Resource *res, *existing;

    res = (struct Clib4Resource *) IExec->OpenResource(RESOURCE_NAME);
    if (res != NULL)
        return res;

    res = static_clib4_resource_new();
    if (res == NULL)
        return NULL;

    /* Two programs may start at once. Publish under Forbid(); if the other one
     * got there first, keep its resource and drop this one. Forbid() is not an
     * exclusion across cores (see the SMP note in the report). */
    IExec->Forbid();
    existing = (struct Clib4Resource *) IExec->OpenResource(RESOURCE_NAME);
    if (existing == NULL)
        IExec->AddResource(res);
    IExec->Permit();

    if (existing != NULL) {
        hashmap_free(res->children);
        hashmap_free(res->uxSocketsMap);
        reent_exit(res->fallbackClib);
        IExec->FreeVec(res->fallbackClib);
        IExec->FreeVec(res);
        return existing;
    }
    return res;
}

static uint32
copyEnvironment(struct Hook *hook, struct envHookData *ehd, struct ScanVarsMsg *message) {
    DECLARE_UTILITYBASE();

    if (message == NULL || message->sv_Name == NULL || IUtility->Strlen(message->sv_Name) == 0) {
        return 0;  // continue search
    }

    if (IUtility->Strlen(message->sv_GDir) <= 4) {
        if (ehd->env_size == ehd->allocated_size) {
            if (!(ehd->r->__environment = realloc(ehd->r->__environment, ehd->allocated_size + ENVIRON_SIZE))) {
                return 1;
            }
            IUtility->ClearMem((char *)ehd->r->__environment + ehd->allocated_size, ENVIRON_SIZE);
            ehd->allocated_size += ENVIRON_SIZE;
        }
        char **env = (char **) hook->h_Data;
        uint32 size = IUtility->Strlen(message->sv_Name) + 1 + message->sv_VarLen + 1 + 1;
        char *buffer = (char *) IExec->AllocVecPooled(ehd->r->__environment_pool, size);
        if (buffer == NULL) {
            return 1;
        }

        IUtility->SNPrintf(buffer, size - 1, "%s=%s", message->sv_Name, message->sv_Var);
        *env = buffer;
        env++;
        hook->h_Data = env;
    }
    return 0;
}

static void
makeEnvironment(struct _clib4 *__clib4) {
    char varbuf[8] = {0};
    uint32 flags = 0;

    if (IDOS->GetVar("EXEC_IMPORT_LOCAL", varbuf, sizeof(varbuf), GVF_LOCAL_ONLY) > 0) {
        flags = GVF_LOCAL_ONLY;
    }

    __clib4->__environment = (char **) calloc(ENVIRON_SIZE, 1);
    if (!__clib4->__environment)
        return;

    flags |= GVF_SCAN_TOPLEVEL;

    __clib4->__environment_pool = IExec->AllocSysObjectTags(ASOT_MEMPOOL,
                                                     ASOPOOL_Puddle,	ENVIRON_SIZE,
                                                     ASOPOOL_Threshold,	ENVIRON_SIZE,
                                                     TAG_DONE);
    if (__clib4->__environment_pool) {
        struct Hook *hook = IExec->AllocSysObjectTags(ASOT_HOOK,
                                               ASOHOOK_Entry, copyEnvironment,
                                               ASOHOOK_Data, __clib4->__environment,
                                               TAG_DONE);
        if (hook != NULL) {
            struct envHookData ehd = {1, ENVIRON_SIZE, __clib4};
            IDOS->ScanVars(hook, flags, &ehd);
            IExec->FreeSysObject(ASOT_HOOK, hook);
        }
    } else {
        /* Failed to allocate pool, cleanup */
        free(__clib4->__environment);
        __clib4->__environment = NULL;
        return;
    }

    __clib4->__environment_lock = __create_recursive_mutex();
}

static void
freeEnvironment(struct _clib4 *__clib4) {
    if (__clib4->__environment_pool != NULL) {
        IExec->FreeSysObject(ASOT_MEMPOOL, __clib4->__environment_pool);
        __clib4->__environment_pool = NULL;
    }
    if (__clib4->__environment_lock != NULL) {
        __delete_mutex(__clib4->__environment_lock);
        __clib4->__environment_lock = NULL;
    }
    free(__clib4->__environment);
    __clib4->__environment = NULL;
}

/*
 * What libOpen() does to a new context, in the same order (see
 * shared_library/clib4.c). crtbegin.c calls this after reent_init() and after
 * pr_UID points at the context. Returns FALSE if the context cannot be set up.
 * The machine type is not read on this path.
 */
BOOL
static_clib4_init(struct _clib4 *__clib4) {
    DECLARE_UTILITYBASE();
    char envbuf[ENVBUF + 1];
    char term_buffer[FILENAME_MAX] = {0};
    struct Process *me = (struct Process *) IExec->FindTask(NULL);
    uint32 pid = IDOS->GetPID(0, GPID_PROCESS);
    uint32 ppid = IDOS->GetPID(0, GPID_PARENT);
    LONG len;

    IUtility->ClearMem(envbuf, ENVBUF + 1);

    __clib4->processId = pid;
    __clib4->self = me;

    /* A custom memory allocator is chosen before clib4_init() runs, because
     * stdlib_memory_init() reads __wof_mem_allocator_type. */
    if ((len = IDOS->GetVar("CLIB4_MEMORY_ALLOCATOR", envbuf, sizeof(envbuf), 0)) >= 0) {
        if (!IUtility->Stricmp(envbuf, "1"))
            __clib4->__wof_mem_allocator_type = WMEM_ALLOCATOR_SIMPLE;
        else if (!IUtility->Stricmp(envbuf, "2"))
            __clib4->__wof_mem_allocator_type = WMEM_ALLOCATOR_BLOCK;
        else if (!IUtility->Stricmp(envbuf, "3"))
            __clib4->__wof_mem_allocator_type = WMEM_ALLOCATOR_STRICT;
        else if (!IUtility->Stricmp(envbuf, "4"))
            __clib4->__wof_mem_allocator_type = WMEM_ALLOCATOR_BLOCK_FAST;
    }

    /* The per-process allocator comes before the resource: building the resource
     * calls malloc(), and malloc() needs the allocator. stdlib_memory_init()
     * finds it already there. */
    if (__clib4->__wmem_allocator == NULL)
        __clib4->__wmem_allocator = wmem_allocator_new(__clib4->__wof_mem_allocator_type);
    if (__clib4->__wmem_allocator == NULL)
        return FALSE;

    if (static_clib4_resource() == NULL)
        return FALSE;

    clib4_init();

    import_pending_fds_for_process(__clib4, pid, ppid);

    makeEnvironment(__clib4);
    if (!__clib4->__environment) {
        __clib4->__environment = empty_env;
        __clib4->__environment_allocated = FALSE;
    }
    else
        __clib4->__environment_allocated = TRUE;

    /* Default terminal mode, as libOpen() sets it. */
    if (getenv("TERM") == NULL) {
        IUtility->Strlcpy(term_buffer, "amiga-clib4", FILENAME_MAX);
        setenv("TERM", term_buffer, true);
    }
    return TRUE;
}

/* What libClose() does, in the same order: clib4_exit() then freeEnvironment(). */
void
static_clib4_exit(struct _clib4 *__clib4) {
    clib4_exit();
    freeEnvironment(__clib4);
}
