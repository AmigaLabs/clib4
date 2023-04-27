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
struct Library *ElfBase;

struct Clib2IFace *IClib2;

struct Library *__UtilityBase;
struct UtilityIFace *__IUtility;

const struct Resident RomTag;

#define MIN_OS_VERSION 52

/* These CTORS/DTORS are clib2's one and they are different than that one received
 * from crtbegin. They are needed because we need to call clib2 constructors as well
 */
static void (*__CTOR_LIST__[1])(void) __attribute__((section(".ctors")));
static void (*__DTOR_LIST__[1])(void) __attribute__((section(".dtors")));
extern void reent_init(void);
extern void reent_exit(void);

struct _clib2 *__global_clib2;
static BPTR clib2SegList;

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

    D(("We have %ld constructors in library", i));
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

static void closeLibraries() {
    if (__UtilityBase != NULL) {
        IExec->CloseLibrary(__UtilityBase);
        __UtilityBase = NULL;
    }

    if (__IUtility != NULL) {
        IExec->DropInterface((struct Interface *) __IUtility);
        __IUtility = NULL;
    }

    if (ElfBase != NULL) {
        IExec->CloseLibrary(ElfBase);
        ElfBase = NULL;
    }

    if (IElf != NULL) {
        IExec->DropInterface((struct Interface *) IElf);
        IElf = NULL;
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

static struct Library *LIB_Open(struct Interface *Self, uint32 version __attribute__((unused))) {
    struct Library *Clib2Base = Self->Data.LibBase;
    D(("LIBOpen"));

    if (version > VERSION) {
        D(("Wrong version library required"));
        return NULL;
    }

    if (!IClib2) {
        IClib2 = (struct Clib2IFace *) IExec->GetInterface(Clib2Base, "main", 1, NULL);
        IExec->DropInterface((struct Interface *) IClib2);
    }

    if (!IDOS) {
        D(("Open DOS Library"));
        DOSBase = IExec->OpenLibrary("dos.library", MIN_OS_VERSION);
        if (DOSBase) {
            IDOS = (struct DOSIFace *) IExec->GetInterface((struct Library *) DOSBase, "main", 1, NULL);
            if (!IDOS) {
                goto out;
            }
        } else
            goto out;
    }

    if (!IElf) {
        D(("Open Elf Library"));
        struct Library *ElfBase = IExec->OpenLibrary("elf.library", MIN_OS_VERSION);
        if (ElfBase) {
            if (ElfBase->lib_Version == 52 && ElfBase->lib_Revision == 1) { // .so stuff doesn't work with pre-52.2
                D(("Elf.library is 52.1. We can't use this version."));
                goto out;
            }

            IElf = (struct ElfIFace *) IExec->GetInterface(ElfBase, "main", 1, NULL);
            if (!IElf) {
                D(("Cannot get IElf interface"));
                goto out;
            }
        } else {
            D(("Cannot open Elf library"));
            goto out;
        }
    }

    if (!__UtilityBase) {
        D(("Open Utility Library"));
        __UtilityBase = IExec->OpenLibrary("utility.library", MIN_OS_VERSION);
        if (__UtilityBase) {
            __IUtility = (struct UtilityIFace *) IExec->GetInterface(__UtilityBase, "main", 1, NULL);
            if (__IUtility == NULL) {
                D(("Cannot get IUtility interface"));
                goto out;
            }

            /* If all libraries are opened correctlty we can initialize clib2 reent structure */
            D(("Initialize clib2 reent structure"));
            reent_init();

            /* After reent structure we can call clib2 constructors */
            D(("Calling clib2 ctors"));
            _start_ctors(__CTOR_LIST__);
            D(("Done. All constructors called"));
        }
        else {
            D(("Cannot open utility.library"));
            goto out;
        }
    }

    Clib2Base->lib_OpenCnt++;
    Clib2Base->lib_Flags &= ~LIBF_DELEXP;

    D(("Exit LIBOpen: Open Count: %ld", Clib2Base->lib_OpenCnt));
    return Clib2Base;

out:

    D(("Jumped into error"));
    /* if we jump in out we need to close all libraries and return NULL */
    closeLibraries();

    return NULL;
}

static BPTR LIB_Expunge(struct Interface *Self) {
    struct Library *Clib2Base = Self->Data.LibBase;
    D(("LIBExpunge"));

    if (Clib2Base->lib_OpenCnt) {
        Clib2Base->lib_Flags |= LIBF_DELEXP;
        return 0;
    }

    D(("Calling reent_exit"));
    reent_exit();
    D(("Done"));

    D(("Calling clib2 dtors"));
    _start_dtors(__DTOR_LIST__);
    D(("Done. All destructors called"));

    D(("Close all libraries "));
    closeLibraries();

    D(("Remove Clib2Base"));
    IExec->Remove((struct Node *) Clib2Base);

    return clib2SegList;
}

static BPTR LIB_Close(struct Interface *Self) {
    struct Library *Clib2Base = Self->Data.LibBase;

    Clib2Base->lib_OpenCnt--;
    D(("LIBClose: Count=%ld", Clib2Base->lib_OpenCnt));
    if ((Clib2Base->lib_OpenCnt == 0) && (Clib2Base->lib_Flags & LIBF_DELEXP)) {
        return LIB_Expunge(Self);
    }

    return 0;
}

static uint32 _manager_Obtain(struct Interface *Self) {
    if (Self)
        return ++Self->Data.RefCount;
    else
        return 0;
}


static uint32 _manager_Release(struct Interface *Self) {
    if (Self)
        return --Self->Data.RefCount;
    else
        return 0;
}

static void *lib_manager_vectors[] = {
        (void *) _manager_Obtain,
        (void *) _manager_Release,
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
    clib2SegList = librarySegment;

    Clib2Base->lib_Revision = REVISION;

    /* Set all Interface pointers to NULL */
    IDOS = NULL;
    IElf = NULL;

    DOSBase = NULL;
    ElfBase = NULL;

    IClib2 = NULL;

    __UtilityBase = NULL;
    __IUtility = NULL;

    return Clib2Base;
}

/* CreateLibrary tag list */
static struct TagItem libCreateTags[] = {
        {CLT_DataSize,   (uint32) (sizeof(struct Library))},
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