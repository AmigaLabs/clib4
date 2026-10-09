/*
 * crtbegin.c - the program start for ARM, with the C library linked statically.
 *
 * The PowerPC crtbegin opens clib4.library and hands the program to its
 * library_start(). A static program has no library: this file does the same
 * work itself. It opens the interfaces the library needs, creates the per-
 * process clib4 context (what libOpen() builds for a shared library), and calls
 * _main(), the routine that runs the program. No register is reserved (the
 * PowerPC crtbegin binds r13 and r2 as globals; on AAPCS those are sp and a
 * caller-saved register, so that device is not used here).
 *
 * The globals below are the ones the static library refers to; they are defined
 * here rather than in shared_library/clib4.c, which a static program does not
 * link.
 *
 * No part of this file is derived from any AmigaOS implementation.
 */

#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <exec/tasks.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/dosextens.h>
#include <workbench/startup.h>
#include <interfaces/exec.h>
#include <interfaces/dos.h>
#include <interfaces/utility.h>

#include <stdint.h>
#include <stddef.h>

/* struct _clib4, the per-process context, is defined by the library's dos.h. */
#include "dos.h"

extern int _main(char *argstr, int arglen,
                 int (*start_main)(int, char **, char **),
                 void (*__EXT_CTOR_LIST__[])(void),
                 void (*__EXT_DTOR_LIST__[])(void),
                 struct WBStartup *sms);
extern BOOL reent_init(struct _clib4 *__clib4, BOOL fallback);
extern void reent_exit(struct _clib4 *__clib4);
extern BOOL static_clib4_init(struct _clib4 *__clib4);  /* stdlib/static_init.c */
extern void static_clib4_exit(struct _clib4 *__clib4);

extern int main(int, char **, char **);

/* The library's globals. The names are the ones the static libc refers to. */
struct ExecBase    *SysBase   = NULL;
struct ExecIFace   *IExec     = NULL;
struct Library     *DOSBase   = NULL;
struct DOSIFace    *IDOS      = NULL;
struct Library     *__UtilityBase = NULL;
struct UtilityIFace *__IUtility = NULL;
struct ElfIFace    *__IElf    = NULL;
struct TimeRequest *TimeReq   = NULL;
struct TimerIFace  *ITimer    = NULL;


/*
 * Constructor and destructor lists. The first entry is the count sentinel that
 * _start_ctors()/_end_ctors() read; crtend.o terminates the lists in the same
 * sections, so the library finds them as __EXT_CTOR_LIST__ / __EXT_DTOR_LIST__.
 */
static void (*__CTOR_LIST__[1])(void) __attribute__((section(".ctors"))) = { (void *) ~0 };
static void (*__DTOR_LIST__[1])(void) __attribute__((section(".dtors"))) = { (void *) ~0 };

#define START_MIN_VERSION 50

static struct Interface *
open_library_interface(struct ExecIFace *iexec, CONST_STRPTR name, ULONG version,
                       struct Library **base_out)
{
    struct Library *library;
    struct Interface *interface;

    library = iexec->OpenLibrary(name, version);
    if (library == NULL)
        return NULL;
    interface = iexec->GetInterface(library, "main", 1, NULL);
    if (interface == NULL) {
        iexec->CloseLibrary(library);
        return NULL;
    }
    *base_out = library;
    return interface;
}

static void
close_library_interface(struct ExecIFace *iexec, struct Interface *interface,
                        struct Library *base)
{
    if (interface != NULL)
        iexec->DropInterface(interface);
    if (base != NULL)
        iexec->CloseLibrary(base);
}

static int
start_program(STRPTR argstring, int32 arglen, struct Library *sysbase)
{
    struct ExecIFace *iexec;
    struct Interface *idos_if = NULL, *iutil_if = NULL;
    struct Library *dos_base = NULL, *util_base = NULL;
    struct _clib4 *clib4 = NULL;
    struct Process *me;
    struct WBStartup *sms = NULL;
    int rc = RETURN_FAIL;

    SysBase = (struct ExecBase *) sysbase;
    iexec = (struct ExecIFace *) SysBase->MainInterface;
    IExec = iexec;
    iexec->Obtain();

    /* A program started from Workbench gets its startup message here. */
    me = (struct Process *) iexec->FindTask(NULL);
    if (!me->pr_CLI) {
        struct MsgPort *mp = &me->pr_MsgPort;
        iexec->WaitPort(mp);
        sms = (struct WBStartup *) iexec->GetMsg(mp);
    }

    idos_if = open_library_interface(iexec, "dos.library", START_MIN_VERSION, &dos_base);
    if (idos_if == NULL) {
        iexec->Alert(AT_Recovery | AG_OpenLib | AO_DOSLib);
        goto out;
    }
    IDOS = (struct DOSIFace *) idos_if;
    DOSBase = dos_base;

    iutil_if = open_library_interface(iexec, "utility.library", START_MIN_VERSION, &util_base);
    if (iutil_if == NULL) {
        ((struct DOSIFace *) IDOS)->Printf("Cannot open utility.library version %ld!\n",
                                           (LONG) START_MIN_VERSION);
        goto out;
    }
    __IUtility = (struct UtilityIFace *) iutil_if;
    __UtilityBase = util_base;

    /* The per-process context: what libOpen() creates for a shared library. It
     * is reached through pr_UID (see getclib4.c), which _main() saves and
     * restores around the program. */
    clib4 = (struct _clib4 *) iexec->AllocVecTags(sizeof(struct _clib4),
                                                  AVT_Type, MEMF_SHARED,
                                                  AVT_ClearWithValue, 0,
                                                  TAG_DONE);
    if (clib4 == NULL || !reent_init(clib4, FALSE)) {
        ((struct DOSIFace *) IDOS)->Printf("Cannot create the clib4 context.\n");
        if (clib4 != NULL)
            iexec->FreeVec(clib4);
        clib4 = NULL;
        goto out;
    }
    clib4->self = me;
    me->pr_UID = (uint32) (uintptr_t) clib4;
    if (!static_clib4_init(clib4)) {
        ((struct DOSIFace *) IDOS)->Printf("Cannot set up the clib4 context.\n");
        me->pr_UID = 0;
        reent_exit(clib4);
        iexec->FreeVec(clib4);
        clib4 = NULL;
        goto out;
    }

    rc = _main(argstring, arglen, main, __CTOR_LIST__, __DTOR_LIST__, sms);

    static_clib4_exit(clib4);
    me->pr_UID = 0;
    reent_exit(clib4);
    iexec->FreeVec(clib4);

out:
    close_library_interface(iexec, iutil_if, util_base);
    __IUtility = NULL;
    __UtilityBase = NULL;
    close_library_interface(iexec, idos_if, dos_base);
    IDOS = NULL;
    DOSBase = NULL;
    iexec->Release();
    return rc;
}

/*
 * The program's entry point. The loader passes the argument string, its length
 * and the exec base, as the PowerPC loader does.
 */
int
_start(STRPTR argstring, int32 arglen, struct Library *sysbase)
{
    return start_program(argstring, arglen, sysbase);
}
