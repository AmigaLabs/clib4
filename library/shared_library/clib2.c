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

#include "dos.h"
#include "c.lib_rev.h"

#include "clib2.h"
#include "debug.h"
#include "uuid.h"

#include "interface.h"

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

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

const struct Resident RomTag;

#define LIBPRI 0
#define LIBNAME "clib2.library"

static struct TimeRequest *openTimer(uint32 unit);
static void closeTimer(struct TimeRequest *tr);

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

struct Clib2Base *libOpen(struct LibraryManagerInterface *Self, uint32 version) {
    D(("LIBOpen"));

    struct Clib2Base *libBase = (struct Clib2Base *) Self->Data.LibBase;

    if (version > VERSION) {
        D(("Wrong version library required"));
        return NULL;
    }

    ++libBase->libNode.lib_OpenCnt;
    libBase->libNode.lib_Flags &= ~LIBF_DELEXP;

    D(("Exit LIBOpen: Open Count: %ld", libBase->libNode.lib_OpenCnt));
    return libBase;
}

BPTR libExpunge(struct LibraryManagerInterface *Self) {
    D(("LIBExpunge"));
    BPTR result = 0;

    D(("Remove Resource"));
    struct Clib2Resource *res = (APTR) IExec->OpenResource(RESOURCE_NAME);
    if (res) {
        hashmap_free(res->uxSocketsMap);
        if (res->fallbackClib) {
            reent_exit(res->fallbackClib, TRUE);
            IExec->FreeVec(res->fallbackClib);
        }

        IExec->RemResource(res);
        IExec->FreeVec(res);
    }

    struct Clib2Base *libBase = (struct Clib2Base *) Self->Data.LibBase;
    if (libBase->libNode.lib_OpenCnt) {
        libBase->libNode.lib_Flags |= LIBF_DELEXP;
        return result;
    }

    D(("Close all libraries "));
    closeLibraries();

    D(("Remove Clib2Base"));
    IExec->Remove(&libBase->libNode.lib_Node);

    result = libBase->SegList;

    D(("Delete library"));
    IExec->DeleteLibrary(&libBase->libNode);

    return result;
}

BPTR libClose(struct LibraryManagerInterface *Self) {
    struct Clib2Base *libBase = (struct Clib2Base *) Self->Data.LibBase;
    ENTER();

    --libBase->libNode.lib_OpenCnt;
    D(("LIBClose: Count=%ld", libBase->libNode.lib_OpenCnt));

    D(("Done %ld", libBase->libNode.lib_OpenCnt));
    if (libBase->libNode.lib_OpenCnt) {
        RETURN(0);
        return 0;
    }

    if (libBase->libNode.lib_Flags & LIBF_DELEXP) {
        SHOWMSG("Call Expunge");
        LEAVE();
        return (BPTR) Self->LibExpunge();
    } else {
        RETURN(0);
        return 0;
    }
}

uint32 clib2Obtain(struct Clib2IFace *Self) {
    return ++Self->Data.RefCount;
}

uint32 clib2Release(struct Clib2IFace *Self) {
    return --Self->Data.RefCount;
}

uint32 libObtain(struct LibraryManagerInterface *Self) {
    //return ++Self->Data.RefCount;
}

uint32 libRelease(struct LibraryManagerInterface *Self) {
    //return --Self->Data.RefCount;
}

int libReserved(void) {
    __CLIB2->_errno = ENOSYS;
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


struct Clib2Base *libInit(struct Clib2Base *libBase, BPTR seglist, struct ExecIFace *const iexec) {
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

    D(("Open Elf Library"));
    struct Library *__ElfBase = IExec->OpenLibrary("elf.library", MIN_OS_VERSION);
    if (__ElfBase) {
        if (__ElfBase->lib_Version == 52 && __ElfBase->lib_Revision == 1) { // .so stuff doesn't work with pre-52.2
            D(("Elf.library is 52.1. We can't use this version."));
            goto out;
        }

        __IElf = (struct ElfIFace *) IExec->GetInterface(__ElfBase, "main", 1, NULL);
        if (!__IElf) {
            D(("Cannot get __IElf interface"));
            goto out;
        }
    } else {
        D(("Cannot open Elf library"));
        goto out;
    }

    D(("Open Utility Library"));
    __UtilityBase = IExec->OpenLibrary("utility.library", MIN_OS_VERSION);
    if (__UtilityBase) {
        __IUtility = (struct UtilityIFace *) IExec->GetInterface(__UtilityBase, "main", 1, NULL);
        if (__IUtility == NULL) {
            D(("Cannot get IUtility interface"));
            goto out;
        }
    } else {
        D(("Cannot open utility.library"));
        goto out;
    }

    /* Open resource */
    struct Clib2Resource *res = (APTR) iexec->OpenResource(RESOURCE_NAME);
    if (!res) {
        res = iexec->AllocVecTags(
                sizeof(struct Clib2Resource),
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
            res->size = sizeof(*res);

            iexec->InitSemaphore(&res->semaphore);
            iexec->NewList(&res->nodes);
            /* Initialize unix sockets hashmap */
            res->uxSocketsMap = hashmap_new(sizeof(struct UnixSocket), 0, 0, 0, unixSocketHash, unixSocketCompare, NULL, NULL);
            /* Initialize fallback clib2 reent structure */
            res->fallbackClib = (struct _clib2 *) iexec->AllocVecTags(sizeof(struct _clib2),
                                                                 AVT_Type, MEMF_SHARED,
                                                                 AVT_ClearWithValue, 0,
                                                                 TAG_DONE);
            reent_init(res->fallbackClib);
            res->fallbackClib->__check_abort_enabled = TRUE;

            iexec->AddResource(res);
        } else {
            goto out;
        }
    }

    return libBase;

out:
    D(("Jumped into error"));
    /* if we jump in out we need to close all libraries and return NULL */
    closeLibraries();

    return NULL;
}

static void *libMmanagerVectors[] = {
        (void *) libObtain,
        (void *) libRelease,
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
        {MIT_VectorTable, (uint32) libMmanagerVectors},
        {MIT_Version,     1},
        {MIT_DataSize,    0},
        {TAG_DONE,        0}
};

extern struct _clib2 *__getClib2(void);

#include "clib2_vectors.h"

static struct TagItem mainTags[] = {
        {MIT_Name,        (uint32) "main"},
        {MIT_VectorTable, (uint32) clib2Vectors},
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
        {CLT_DataSize,   (uint32)(sizeof(struct Clib2Base))},
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