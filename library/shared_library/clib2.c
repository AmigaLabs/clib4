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
#include <aio.h>
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
#include <sys/utsname.h>
#include <sys/uio.h>

#include "dos.h"
#include "c.lib_rev.h"

#include "clib2.h"
#include "debug.h"

struct ExecBase *SysBase;
struct ExecIFace *IExec;

struct Library *DOSBase;
struct DOSIFace *IDOS;

struct ElfIFace *IElf;
struct TimeRequest *TimeReq;

struct TimerIFace *ITimer;
struct TimezoneIFace *ITimezone;

struct LocaleIFace *ILocale;
struct Clib2IFace *IClib2;

struct Library* __UtilityBase;
struct UtilityIFace* __IUtility;

BPTR clib2seglist;

const struct Resident RomTag;

/* These CTORS/DTORS are clib2's one and they are different than that one received
 * from crtbegin. They are needed because we need to call clib2 constructors as well
 */
static void (*__CTOR_LIST__[1])(void) __attribute__((section(".ctors")));
static void (*__DTOR_LIST__[1])(void) __attribute__((section(".dtors")));

extern void reent_init(void);
extern void reent_exit(void);

struct _clib2 *__global_clib2;

int32
_start(char *args, int arglen, struct ExecBase *sysbase) {
    (void) (args);
    (void) (arglen);
    (void) (sysbase);

    return -1;
}

static void
_start_ctors(void (*__CTOR_LIST__[])(void)) {
    int i = 0;

    while (__CTOR_LIST__[i + 1]) {
        i++;
    }
    SHOWVALUE(i);
    while (i > 0) {
        D(("Calling ctor %ld", i));
        __CTOR_LIST__[i--]();
    }
}

static void
_start_dtors(void (*__DTOR_LIST__[])(void)) {
    int i = 1;

    while (__DTOR_LIST__[i]) {
        D(("Calling dtor %ld", i));
        __DTOR_LIST__[i++]();
    }
}

static struct TimeRequest *open_timer(uint32 unit) {
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

static void close_timer(struct TimeRequest *tr) {
    if (tr != NULL) {
        struct MsgPort *mp = tr->Request.io_Message.mn_ReplyPort;

        IExec->CloseDevice((struct IORequest *) tr);

        IExec->FreeSysObject(ASOT_IOREQUEST, tr);
        IExec->FreeSysObject(ASOT_PORT, mp);
    }
}

static struct Library *LIB_Open(struct Interface *Self, uint32 version __attribute__((unused))) {
    struct Library *Clib2Base = Self->Data.LibBase;
    if (!IClib2) {
        IClib2 = (struct Clib2IFace *) IExec->GetInterface(Clib2Base, "main", 1, NULL);
        IExec->DropInterface((struct Interface *) IClib2);
    }
    if (!IDOS) {
        DOSBase = IExec->OpenLibrary("dos.library", 50);
        if (DOSBase) {
            IDOS = (struct DOSIFace *) IExec->GetInterface((struct Library *) DOSBase, "main", 1, NULL);
            if (!IDOS) {
                IExec->CloseLibrary((struct Library *) DOSBase);
                DOSBase = NULL;
            }
        }
    }

    if (!IElf) {
        struct Library *ElfBase = IExec->OpenLibrary("elf.library", 52);
        if (ElfBase) {
            if (ElfBase->lib_Version == 52 && ElfBase->lib_Revision == 1) // shlib stuff doesn't work with pre-52.2
            {
                IExec->CloseLibrary(ElfBase);
                ElfBase = NULL;
            }

            if (ElfBase) {
                IElf = (struct ElfIFace *) IExec->GetInterface(ElfBase, "main", 1, NULL);
                if (!IElf) {
                    IExec->CloseLibrary(ElfBase);
                    ElfBase = NULL;
                }
            }
        }
    }

    __UtilityBase = IExec->OpenLibrary("utility.library", 52);
    __IUtility = (struct UtilityIFace *) IExec->GetInterface(__UtilityBase, "main", 1, NULL);
    if (__IUtility == NULL) {
        IExec->CloseLibrary(__UtilityBase);
        __IUtility = NULL;
    }

    reent_init();

    D(("Calling clib2 ctors"));
    _start_ctors(__CTOR_LIST__);

    Clib2Base->lib_OpenCnt++;
    Clib2Base->lib_Flags &= ~LIBF_DELEXP;

    return Clib2Base;
}

static BPTR LIB_Expunge(struct Interface *Self) {
    struct Library *Clib2Base = Self->Data.LibBase;

    reent_exit();

    _start_dtors(__DTOR_LIST__);

    if (Clib2Base->lib_OpenCnt) {
        Clib2Base->lib_Flags |= LIBF_DELEXP;
        return 0;
    }

    if (clib2seglist) {
        IExec->Remove((struct Node *) Clib2Base);
        IExec->DeleteLibrary(Clib2Base);
    }
    return clib2seglist;
}

static BPTR LIB_Close(struct Interface *Self) {
    struct Library *Clib2Base = Self->Data.LibBase;

    Clib2Base->lib_OpenCnt--;
    if ((Clib2Base->lib_OpenCnt == 0) && (Clib2Base->lib_Flags & LIBF_DELEXP)) {
        return LIB_Expunge(Self);
    }

    return 0;
}

static uint32 _generic_Obtain(struct Interface *Self) {
    return ++Self->Data.RefCount;
}


static uint32 _generic_Release(struct Interface *Self) {
    return --Self->Data.RefCount;
}

static void *lib_manager_vectors[] = {
        (void *) _generic_Obtain,
        (void *) _generic_Release,
        (void *) 0,
        (void *) 0,
        (void *) LIB_Open,
        (void *) LIB_Close,
        (void *) LIB_Expunge,
        (void *) 0,
        (void *) -1,
};

static struct TagItem libmanagerTags[] = {
        {MIT_Name,        (uint32) "__library"},
        {MIT_VectorTable, (uint32) lib_manager_vectors},
        {MIT_Version,     1},
        {TAG_DONE,        0}
};

uint32 LIB_Obtain(struct Interface *Self) {
    return ++Self->Data.RefCount;
}

uint32 LIB_Release(struct Interface *Self) {
    return --Self->Data.RefCount;
}

int LIB_Reserved(void) {
    __global_clib2->_errno = ENOSYS;
    return -1;
}

extern struct _clib2 *__getclib2(void);

#include "clib2_vectors.h"

static struct TagItem mainTags[] = {
        {MIT_Name,        (uint32) "main"},
        {MIT_VectorTable, (uint32) main_vectors},
        {MIT_Version,     1},
        {TAG_DONE,        0}
};

/* MLT_INTERFACES array */
static uint32 libInterfaces[] = {
        (uint32) libmanagerTags,
        (uint32) mainTags,
        0
};

struct Library *LIB_Init(struct Library *Clib2Base, BPTR librarySegment, struct ExecIFace *const iexec) {
    APTR pool = NULL;

    SysBase = (struct ExecBase *) iexec->Data.LibBase;
    IExec = iexec;
    clib2seglist = librarySegment;

    Clib2Base->lib_Revision = REVISION;

    /* Set all Interface pointers to NULL */
    IDOS = NULL;
    IElf = NULL;
    ITimer = NULL;
    TimeReq = NULL;
    DOSBase = NULL;
    ITimezone = NULL;
    ILocale = NULL;
    IClib2 = NULL;

    return Clib2Base;
}

/* CreateLibrary tag list */
static struct TagItem libCreateTags[] = {
        {CLT_DataSize,   (uint32)(sizeof(struct Library))},
        {CLT_Interfaces, (uint32) libInterfaces},
        {CLT_InitFunc,   (uint32) LIB_Init},
        {TAG_DONE,       0}
};

const struct Resident RomTag = {
        RTC_MATCHWORD,
        (struct Resident *) &RomTag,
        (struct Resident *) &RomTag + 1,
        RTF_NATIVE | RTF_AUTOINIT | RTF_COLDSTART,
        VERSION,
        NT_LIBRARY,
        100, /* PRI, usually not needed unless you're resident */
        "clib2.library",
        VSTRING,
        (APTR) libCreateTags
};

int
library_start(char *argstr,
              int arglen,
              struct Library **_DOSBase,
              struct DOSIFace **_IDOS,
              int (*start_main)(int, char **),
              void (*__EXT_CTOR_LIST__[])(void),
              void (*__EXT_DTOR_LIST__[])(void)) {

    int result = _main(argstr, arglen, _DOSBase, _IDOS, start_main, __EXT_CTOR_LIST__, __EXT_DTOR_LIST__);

    return result;
}