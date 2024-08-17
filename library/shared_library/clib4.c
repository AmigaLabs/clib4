#define __NOLIBBASE__
#define __NOGLOBALIFACE__
#define shared_comp
#undef __USE_INLINE__

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/elf.h>
#include <proto/locale.h>
#include <proto/timer.h>
#include <proto/timezone.h>

#include <arpa/inet.h>
#include <argz.h>
#include <assert.h>
#include <complex.h>
#include <crypt.h>
#include <ctype.h>
#include <dirent.h>
#include <dlfcn.h>
#include <envz.h>
#include <err.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <ftw.h>
#include <getopt.h>
#include <glob.h>
#include <grp.h>
#include <iconv.h>
#include <ifaddrs.h>
#include <langinfo.h>
#include <libgen.h>
#include <locale.h>
#include <malloc.h>
#include <math.h>
#include <netdb.h>
#include <nl_types.h>
#include <poll.h>
#include <pthread.h>
#include <pwd.h>
#include <regex.h>
#include <resolv.h>
#include <search.h>
#include <semaphore.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <termcap.h>
#include <termios.h>
#include <tgmath.h>
#include <time.h>
#include <uchar.h>
#include <ulimit.h>
#include <unistd.h>
#include <utime.h>
#include <wchar.h>
#include <wctype.h>
#include <wctype.h>
#include <sys/byteswap.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/iconvnls.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/msg.h>
#include <sys/resource.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/syslog.h>
#include <sys/systeminfo.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/uio.h>

#include "../dos.h"
#include "c.lib_rev.h"

#include "clib4.h"
#include "uuid.h"

#include "interface.h"

/* This is variable defines where to start to bind unix local ports using inet addresses */
struct UnixSocket {
    int            port;
    struct fd     *fd;
    char           name[PATH_MAX];
};

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

struct ExecBase *SysBase = 0;
struct ExecIFace *IExec = 0;

struct Library *DOSBase = 0;
struct DOSIFace *IDOS = 0;

struct ElfIFace *__IElf = 0;
struct Library *__ElfBase = 0;

struct TimeRequest *TimeReq = 0;
struct TimerIFace *ITimer = 0;

struct Library *__UtilityBase = 0;
struct UtilityIFace *__IUtility = 0;

struct Clib4IFace *IClib4 = 0;
struct Clib4Library *Clib4Base = 0;

const struct Resident RomTag;

#define LIBPRI 0
#define LIBNAME "clib4.library"

static struct TimeRequest *openTimer(uint32 unit);
static void closeTimer(struct TimeRequest *tr);
static int32 getDebugLevel(struct ExecBase *sysbase);

extern void reent_exit(struct _clib4 *__clib4, BOOL fallback);
extern void reent_init(struct _clib4 *__clib4);

#if DEBUG == 1
#undef D
#define D(x) (x)
#undef DebugPrintF
#define bug IExec->DebugPrintF
#else
#define D(x) ;
#endif

int32
_start(STRPTR args, int32 arglen, struct ExecBase *sysbase) {
    (void) (args);
    (void) (arglen);
    (void) (sysbase);

    return RETURN_FAIL;
}

static void closeLibraries() {
    if (TimeReq != NULL) {
        closeTimer(TimeReq);
        TimeReq = NULL;
    }

    if (__UtilityBase != NULL) {
        IExec->CloseLibrary(__UtilityBase);
        __UtilityBase = NULL;
    }

    if (__IUtility != NULL) {
        IExec->DropInterface((struct Interface *) __IUtility);
        __IUtility = NULL;
    }

    if (__ElfBase != NULL) {
        IExec->CloseLibrary(__ElfBase);
        __ElfBase = NULL;
    }

    if (__IElf != NULL) {
        IExec->DropInterface((struct Interface *) __IElf);
        __IElf = NULL;
    }

    if (DOSBase != NULL) {
        IExec->CloseLibrary(DOSBase);
        DOSBase = NULL;
    }

    if (IDOS != NULL) {
        IExec->DropInterface((struct Interface *) IDOS);
        IDOS = NULL;
    }
}

struct Clib4Library *libOpen(struct LibraryManagerInterface *Self, uint32 version) {
    if (version > VERSION) {
        return NULL;
    }

    struct Clib4Library *libBase = (struct Clib4Library *) Self->Data.LibBase;
    if (!IClib4) {
        D(bug("(libOpen) IClib4 is NULL. Get interface\n"));
        IClib4 = (struct Clib4IFace *) IExec->GetInterface((struct Library *) libBase, "main", 1, NULL);
        D(bug("(libOpen) DropInterface\n"));
        IExec->DropInterface((struct Interface *)IClib4);
    }

    ++libBase->libNode.lib_OpenCnt;
    libBase->libNode.lib_Flags &= ~LIBF_DELEXP;

    struct Clib4Resource *res = (APTR) IExec->OpenResource(RESOURCE_NAME);
    if (res) {
        struct Clib4Node c2n;
        char varbuf[8] = {0};
        char uuid[UUID4_LEN + 1] = {0};
        uint32 pid = IDOS->GetPID(0, GPID_PROCESS);
        uint32 ppid = IDOS->GetPID(0, GPID_PARENT);

        uuid4_generate(c2n.uuid);
        c2n.pid = pid;
        c2n.pPid = ppid;
        c2n.errNo = 0;
        c2n.undo = 0;
        /* Initialize processes hashmap */
        c2n.spawnedProcesses = hashmap_new(sizeof(struct Clib4Children), 0, 0, 0, clib4IntHash, clib4ProcessCompare, NULL, NULL);
        D(bug("(libOpen) c2n.pid = %ld\n", c2n.pid));
        D(bug("(libOpen) c2n.pPid = %ld\n", c2n.pPid));
        D(bug("(libOpen) c2n.uuid = %s\n", c2n.uuid));
        hashmap_set(res->children, &c2n);

        if (IDOS->GetVar("DISABLE_CLIB4_OPTIMIZATIONS", varbuf, sizeof(varbuf), 0) <= 0) {
            D(bug("(libOpen) Enabling clib4 optimizations\n"));
            switch (res->cpufamily) {
#ifdef __SPE__
                case CPUFAMILY_E500:
                    D(bug("(libOpen) Using SPE family functions\n"));
                    IClib4->setjmp = setjmp_spe;
                    IClib4->longjmp = longjmp_spe;
                    IClib4->_longjmp = _longjmp_spe;
                    IClib4->_setjmp = _setjmp_spe;
                    IClib4->__sigsetjmp = __sigsetjmp_spe;
                    IClib4->siglongjmp = siglongjmp_spe;
                    IClib4->strlen = __strlen_e500;
                    IClib4->strcpy = __strcpy_e500;
                    IClib4->strcmp = __strcmp_e500;
                    IClib4->memcmp = __memcmp_e500;
                    break;
#endif
                case CPUFAMILY_4XX:
                    D(bug("(libOpen) Using 4XX family functions\n"));
                    IClib4->strlen = __strlen440;
                    IClib4->strcpy = __strcpy440;
                    IClib4->strcmp = __strcmp440;
                    IClib4->memcmp = __memcmp440;
                    IClib4->memchr = __memchr440;
                    IClib4->strncmp = __strncmp440;
                    IClib4->strrchr = __strrchr440;
                    IClib4->strchr = __strchr440;
                    break;
                default:
                    if (res->altivec) {
                        D(bug("(libOpen) Using Altivec family functions\n"));
                        IClib4->setjmp = setjmp_altivec;
                        IClib4->longjmp = longjmp_altivec;
                        IClib4->strcpy = vec_strcpy;
                        IClib4->memcmp = vec_memcmp;
                        IClib4->bzero = vec_bzero;
                        IClib4->bcopy = vec_bcopy;
#ifdef SLOWER_ALTIVEC_FUNCTIONS
                        IClib4->memchr = vec_memchr;
                        IClib4->strchr = vec_strchr;
#else
                        IClib4->strchr = glibc_strchr; // glibc_strchr is faster than ppc one on qemu/G4
#endif
                    } else {
                        D(bug("(libOpen) Using default family functions\n"));
                    }
            }
        }
        else {
            D(bug("(libOpen) Disabling all clib4 optimizations\n"));
        }
    }
    return libBase;
}

BPTR libExpunge(struct LibraryManagerInterface *Self) {
    BPTR result = 0;

    struct Clib4Resource *res = (APTR) IExec->OpenResource(RESOURCE_NAME);
    if (res) {
        IPCMapUninit(&res->shmcx.keymap);
        IPCMapUninit(&res->msgcx.keymap);
        IPCMapUninit(&res->semcx.keymap);
        hashmap_free(res->uxSocketsMap);

        size_t iter = 0;
        void *item;
        while (hashmap_iter(res->children, &iter, &item)) {
            const struct Clib4Node *node = item;
            if (node->undo)
                IExec->FreeVec(node->undo);
        }

        hashmap_free(res->children);
        if (res->fallbackClib) {
            reent_exit(res->fallbackClib, TRUE);
        }

        IExec->RemResource(res);
        IExec->FreeVec(res);
    }

    struct Clib4Library *libBase = (struct Clib4Library *) Self->Data.LibBase;
    if (libBase->libNode.lib_OpenCnt) {
        libBase->libNode.lib_Flags |= LIBF_DELEXP;
        return result;
    }

    closeLibraries();

    IExec->Remove(&libBase->libNode.lib_Node);
    result = libBase->SegList;
    IExec->DeleteLibrary(&libBase->libNode);

    return result;
}

BPTR libClose(struct LibraryManagerInterface *Self) {
    struct Clib4Library *libBase = (struct Clib4Library *) Self->Data.LibBase;

    struct Clib4Resource *res = (APTR) IExec->OpenResource(RESOURCE_NAME);
    if (res) {
        uint32 pid = IDOS->GetPID(0, GPID_PROCESS);
        size_t iter = 0;
        void *item;
        while (hashmap_iter(res->children, &iter, &item)) {
            const struct Clib4Node *node = item;
            if (node->pid == pid) {
                /* Remove spawnedProcess hashmap */
                if (node->spawnedProcesses != NULL) {
                    hashmap_free(node->spawnedProcesses);
                }
                if (node->undo)
                    IExec->FreeVec(node->undo);
                hashmap_delete(res->children, node);
                break;
            }
        }
    }

    --libBase->libNode.lib_OpenCnt;

    if (libBase->libNode.lib_OpenCnt) {
        return 0;
    }

    if (libBase->libNode.lib_Flags & LIBF_DELEXP) {
        return (BPTR) Self->LibExpunge();
    } else {
        return 0;
    }
}

uint32 clib4Obtain(struct Clib4IFace *Self) {
    return ++Self->Data.RefCount;
}

uint32 clib4Release(struct Clib4IFace *Self) {
    return --Self->Data.RefCount;
}

/* ------------------- Manager Interface ------------------------ */
/* These are generic. Replace if you need more fancy stuff */
static uint32 _manager_Obtain(struct LibraryManagerInterface *Self) {
    uint32 res;
    __asm__ __volatile__(
            "1:	lwarx	%0,0,%1\n"
            "addic	%0,%0,1\n"
            "stwcx.	%0,0,%1\n"
            "bne-	1b"
            : "=&r" (res)
            : "r" (&Self->Data.RefCount)
            : "cc", "memory");

    return res;
}

static uint32 _manager_Release(struct LibraryManagerInterface *Self) {
    uint32 res;
    __asm__ __volatile__(
            "1:	lwarx	%0,0,%1\n"
            "addic	%0,%0,-1\n"
            "stwcx.	%0,0,%1\n"
            "bne-	1b"
            : "=&r" (res)
            : "r" (&Self->Data.RefCount)
            : "cc", "memory");

    return res;
}


int libReserved(void) {
    __CLIB4->_errno = ENOSYS;
    return -1;
}

uint64_t
unixSocketHash(const void *item, uint64_t seed0, uint64_t seed1) {
    const struct UnixSocket *unixSocket = item;
    return hashmap_xxhash3(unixSocket->name, strlen(unixSocket->name), seed0, seed1);
}

int
unixSocketCompare(const void *a, const void *b, void *udata) {
    const struct UnixSocket *ua = a;
    const struct UnixSocket *ub = b;
    return strcmp(ua->name, ub->name);
}

uint64_t
clib4IntHash(const void *item, uint64_t seed0, uint64_t seed1) {
    return hashmap_xxhash3(item, sizeof(int), seed0, seed1);
}


uint64_t
clib4NodeHash(const void *item, uint64_t seed0, uint64_t seed1) {
    const struct Clib4Node *node = item;
    return hashmap_xxhash3(node->uuid, strlen(node->uuid), seed0, seed1);
}

int
clib4NodeCompare(const void *a, const void *b, void *udata) {
    const struct Clib4Node *ua = a;
    const struct Clib4Node *ub = b;
    return strcmp(ua->uuid, ub->uuid);
}

int
clib4ProcessCompare(const void *a, const void *b, void *udata) {
    const struct Clib4Children *ua = a;
    const struct Clib4Children *ub = b;
    return ua->pid - ub->pid;
}

struct Clib4Library *libInit(struct Clib4Library *libBase, BPTR seglist, struct ExecIFace *const iexec) {
    libBase->libNode.lib_Node.ln_Type = NT_LIBRARY;
    libBase->libNode.lib_Node.ln_Pri = LIBPRI;
    libBase->libNode.lib_Node.ln_Name = LIBNAME;
    libBase->libNode.lib_Flags = LIBF_SUMUSED | LIBF_CHANGED;
    libBase->libNode.lib_Version = VERSION;
    libBase->libNode.lib_Revision = REVISION;
    libBase->libNode.lib_IdString = VSTRING;
    libBase->SegList = seglist;

    SysBase = (struct ExecBase *) iexec->Data.LibBase;
    IExec = iexec;

    /* Open libraries */
    DOSBase = IExec->OpenLibrary("dos.library", MIN_OS_VERSION);
    if (DOSBase) {
        IDOS = (struct DOSIFace *) IExec->GetInterface((struct Library *) DOSBase, "main", 1, NULL);
        if (!IDOS) {
            goto out;
        }
    } else
        goto out;

    TimeReq = openTimer(UNIT_MICROHZ);
    if (!TimeReq) {
        goto out;
    }

    struct Device *TimerBase = TimeReq->Request.io_Device;
    ITimer = (struct TimerIFace *) IExec->GetInterface((struct Library *) TimerBase, "main", 1, NULL);
    if (!ITimer) {
        goto out;
    }

    struct Library *__ElfBase = IExec->OpenLibrary("elf.library", MIN_OS_VERSION);
    if (__ElfBase) {
        if (__ElfBase->lib_Version == 52 && __ElfBase->lib_Revision == 1) { // .so stuff doesn't work with pre-52.2
            goto out;
        }

        __IElf = (struct ElfIFace *) IExec->GetInterface(__ElfBase, "main", 1, NULL);
        if (!__IElf) {
            goto out;
        }
    } else {
        goto out;
    }

    __UtilityBase = IExec->OpenLibrary("utility.library", MIN_OS_VERSION);
    if (__UtilityBase) {
        __IUtility = (struct UtilityIFace *) IExec->GetInterface(__UtilityBase, "main", 1, NULL);
        if (__IUtility == NULL) {
            goto out;
        }
    } else {
        goto out;
    }

    /* Open resource */
    struct Clib4Resource *res = (APTR) iexec->OpenResource(RESOURCE_NAME);
    if (!res) {
        res = iexec->AllocVecTags(
                sizeof(struct Clib4Resource),
                AVT_Type, MEMF_SHARED,
                AVT_ClearWithValue, 0,
                AVT_Lock, TRUE,
                TAG_END);

        if (res) {
            res->resource.lib_Version = VERSION;
            res->resource.lib_Revision = REVISION;
            res->resource.lib_IdString = (STRPTR) RESOURCE_NAME;
            res->resource.lib_Node.ln_Name = (STRPTR) RESOURCE_NAME;
            res->resource.lib_Node.ln_Type = NT_RESOURCE;

            iexec->InitSemaphore(&res->semaphore);
            res->debugLevel = getDebugLevel(SysBase);
            D(bug("(libInit) Current Exec debug level: %ld\n", res->debugLevel));
            /* Initialize clib4 children hashmap */
            res->children = hashmap_new(sizeof(struct Clib4Node), 0, 0, 0, clib4NodeHash, clib4NodeCompare, NULL, NULL);
            /* Initialize unix sockets hashmap */
            res->uxSocketsMap = hashmap_new(sizeof(struct UnixSocket), 0, 0, 0, unixSocketHash, unixSocketCompare, NULL, NULL);

            /* Initialize fallback clib4 reent structure */
            res->fallbackClib = (struct _clib4 *) iexec->AllocVecTags(sizeof(struct _clib4),
                                                                      AVT_Type, MEMF_SHARED,
                                                                      AVT_ClearWithValue, 0,
                                                                      TAG_DONE);
            reent_init(res->fallbackClib);
            res->fallbackClib->self = (struct Process *) IExec->FindTask(NULL);
            res->fallbackClib->__check_abort_enabled = TRUE;
            res->fallbackClib->__fully_initialized = TRUE;

            /* Init SYSV structures */
            IPCMapInit(&res->shmcx.keymap);
            res->shmcx.totshm = 0;
            res->shmcx.shmmax = DEF_SHMMAX;
            res->msgcx.qsizemax = DEF_QSIZEMAX;
            IPCMapInit(&res->msgcx.keymap);
            IPCMapInit(&res->semcx.keymap);

            IExec->GetCPUInfoTags(
                    GCIT_VectorUnit, &res->altivec,
                    GCIT_Family, &res->cpufamily,
                    TAG_DONE);

            res->size = sizeof(*res);
            iexec->AddResource(res);
        } else {
            goto out;
        }
    }

    Clib4Base = libBase;

    return libBase;

out:
    /* if we jump in out we need to close all libraries and return NULL */
    closeLibraries();

    return NULL;
}

static void *libManagerVectors[] = {
        (void *) _manager_Obtain,
        (void *) _manager_Release,
        (void *) 0,
        (void *) 0,
        (void *) libOpen,
        (void *) libClose,
        (void *) libExpunge,
        (void *) 0,
        (void *) (APTR) - 1,
};

static struct TagItem libManagerTags[] = {
        {MIT_Name,        (uint32) "__library"},
        {MIT_VectorTable, (uint32) libManagerVectors},
        {MIT_Version,     1},
        {MIT_DataSize,    0},
        {TAG_DONE,        0}
};

extern struct _clib4 *__getClib4(void);

#include "clib4_vectors.h"

static struct TagItem mainTags[] = {
        {MIT_Name,        (uint32) "main"},
        {MIT_VectorTable, (uint32) clib4Vectors},
        {MIT_Version,     1},
        {MIT_DataSize,    0},
        {TAG_DONE,        0}
};

/* MLT_INTERFACES array */
static uint32 libInterfaces[] = {
        (uint32) libManagerTags,
        (uint32) mainTags,
        0
};

/* CreateLibrary tag list */
static struct TagItem libCreateTags[] = {
        {CLT_DataSize,   (uint32)(sizeof(struct Clib4Library))},
        {CLT_Interfaces, (uint32) libInterfaces},
        {CLT_InitFunc,   (uint32) libInit},
        {TAG_DONE,       0}
};

const struct Resident __attribute__((used)) RomTag = {
        RTC_MATCHWORD,
        (struct Resident *) &RomTag,
        (struct Resident *) &RomTag + 1,
        RTF_NATIVE | RTF_AUTOINIT,
        VERSION,
        NT_LIBRARY,
        LIBPRI, /* PRI, usually not needed unless you're resident */
        LIBNAME,
        VSTRING,
        (APTR) libCreateTags
};

int
library_start(char *argstr,
              int arglen,
              int (*start_main)(int, char **),
              void (*__EXT_CTOR_LIST__[])(void),
              void (*__EXT_DTOR_LIST__[])(void)) {

    int result = _main(argstr, arglen, start_main, __EXT_CTOR_LIST__, __EXT_DTOR_LIST__);

    return result;
}

/************** STATIC FUNCTIONS ***********************/
static int32 getDebugLevel(struct ExecBase *sysbase) {
    struct ExecIFace *iexec = (APTR) sysbase->MainInterface;
    int32  debugLevel = 0;

    struct DebugIFace *idebug = (struct DebugIFace *) iexec->GetInterface(&sysbase->LibNode, "debug", 1, NULL);

    if (idebug) {
        debugLevel = idebug->GetDebugLevel();
        iexec->DropInterface((struct Interface *) idebug);
    }
    return debugLevel;
}

static struct TimeRequest *openTimer(uint32 unit) {
    struct MsgPort *mp;
    struct TimeRequest *tr;

    mp = IExec->AllocSysObjectTags(ASOT_PORT,
                                   ASOPORT_AllocSig, FALSE,
                                   ASOPORT_Signal, SIGB_SINGLE,
                                   TAG_END);

    tr = IExec->AllocSysObjectTags(ASOT_IOREQUEST,
                                   ASOIOR_ReplyPort, mp,
                                   ASOIOR_Size, sizeof(struct TimeRequest),
                                   TAG_END);

    if (tr == NULL) {
        IExec->FreeSysObject(ASOT_PORT, mp);
        return NULL;
    }

    if (IExec->OpenDevice(TIMERNAME, unit, (struct IORequest *) tr, 0) != 0) {
        IExec->FreeSysObject(ASOT_IOREQUEST, tr);
        IExec->FreeSysObject(ASOT_PORT, mp);
        return NULL;
    }

    return tr;
}

static void closeTimer(struct TimeRequest *tr) {
    if (tr != NULL) {
        struct MsgPort *mp = tr->Request.io_Message.mn_ReplyPort;

        IExec->CloseDevice((struct IORequest *) tr);

        IExec->FreeSysObject(ASOT_IOREQUEST, tr);
        IExec->FreeSysObject(ASOT_PORT, mp);
    }
}